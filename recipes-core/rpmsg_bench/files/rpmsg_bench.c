#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define FRAME_HDR_SIZE 16u
#define MAX_PAYLOAD    496u /* RPMSG_BUFFER_SIZE (512) - rpmsg_hdr (16) */
#define RX_BUF_SIZE    4096u
#define WARMUP_COUNT   100u

struct frame_hdr {
	uint32_t seq;
	uint32_t len;
	uint64_t tx_ns;
};

struct cfg {
	const char *dev;
	uint32_t count;
	uint32_t window;
	int timeout_ms;
};

struct result {
	uint32_t sent;
	uint32_t received;
	uint32_t lost;
	uint32_t out_of_order;
	uint32_t corrupt;
	double elapsed_s;
	uint64_t *rtt_ns; /* latency mode only */
	uint32_t rtt_cap;
};

struct rx {
	uint8_t buf[RX_BUF_SIZE];
	uint32_t fill;
	uint32_t expected;
};

static const uint32_t sizes[] = { 16, 64, 256, 496 };

static uint64_t now_ns(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void fill_frame(uint8_t *buf, uint32_t seq, uint32_t len)
{
	struct frame_hdr h = { .seq = seq, .len = len, .tx_ns = 0 };

	for (uint32_t i = FRAME_HDR_SIZE; i < len; i++)
		buf[i] = (uint8_t)(seq + i);

	h.tx_ns = now_ns();
	memcpy(buf, &h, sizeof(h));
}

static int pattern_ok(const uint8_t *buf, uint32_t seq, uint32_t len)
{
	for (uint32_t i = FRAME_HDR_SIZE; i < len; i++)
		if (buf[i] != (uint8_t)(seq + i))
			return 0;
	return 1;
}

static int send_frame(int fd, const uint8_t *buf, uint32_t len)
{
	ssize_t n = write(fd, buf, len);

	return (n == (ssize_t)len) ? 0 : -1;
}

/* Returns 1 if data was read, 0 on timeout, -1 on error. */
static int rx_read(int fd, struct rx *rx, int timeout_ms)
{
	struct pollfd p = { .fd = fd, .events = POLLIN };
	ssize_t n;
	int r;

	r = poll(&p, 1, timeout_ms);
	if (r <= 0)
		return r;

	if (rx->fill >= RX_BUF_SIZE)
		return -1;

	n = read(fd, rx->buf + rx->fill, RX_BUF_SIZE - rx->fill);
	if (n <= 0)
		return -1;

	rx->fill += (uint32_t)n;
	return 1;
}

/* The tty may merge or split rpmsg messages, so frames are parsed from a byte stream. */
static int rx_drain(struct rx *rx, struct result *res)
{
	int frames = 0;

	while (rx->fill >= FRAME_HDR_SIZE) {
		struct frame_hdr h;

		memcpy(&h, rx->buf, sizeof(h));
		if (h.len < FRAME_HDR_SIZE || h.len > MAX_PAYLOAD)
			return -1;
		if (rx->fill < h.len)
			break;

		if (h.seq < rx->expected)
			res->out_of_order++;
		else
			rx->expected = h.seq + 1;

		if (!pattern_ok(rx->buf, h.seq, h.len))
			res->corrupt++;

		if (res->rtt_ns && res->received < res->rtt_cap)
			res->rtt_ns[res->received] = now_ns() - h.tx_ns;

		res->received++;
		memmove(rx->buf, rx->buf + h.len, rx->fill - h.len);
		rx->fill -= h.len;
		frames++;
	}

	return frames;
}

static int run_latency(int fd, const struct cfg *c, uint32_t size, struct result *res)
{
	uint8_t tx[MAX_PAYLOAD];
	struct rx rx = { .fill = 0, .expected = 0 };
	uint64_t t0;
	uint32_t seq;

	memset(res, 0, sizeof(*res));
	res->rtt_ns = malloc(c->count * sizeof(*res->rtt_ns));
	if (!res->rtt_ns)
		return -1;
	res->rtt_cap = c->count;

	t0 = now_ns();
	for (seq = 0; seq < c->count; seq++) {
		fill_frame(tx, seq, size);
		if (send_frame(fd, tx, size))
			break;
		res->sent++;

		while (res->received <= seq) {
			if (rx_read(fd, &rx, c->timeout_ms) <= 0)
				goto done;
			if (rx_drain(&rx, res) < 0)
				goto done;
		}
	}

done:
	res->elapsed_s = (double)(now_ns() - t0) / 1e9;
	res->lost = res->sent > res->received ? res->sent - res->received : 0;
	return 0;
}

static int run_throughput(int fd, const struct cfg *c, uint32_t size, struct result *res)
{
	uint8_t tx[MAX_PAYLOAD];
	struct rx rx = { .fill = 0, .expected = 0 };
	uint64_t t0;

	memset(res, 0, sizeof(*res));

	t0 = now_ns();
	while (res->received < c->count) {
		uint32_t inflight = res->sent > res->received ? res->sent - res->received : 0;

		while (res->sent < c->count && inflight < c->window) {
			fill_frame(tx, res->sent, size);
			if (send_frame(fd, tx, size))
				goto done;
			res->sent++;
			inflight++;
		}

		if (rx_read(fd, &rx, c->timeout_ms) <= 0)
			break;
		if (rx_drain(&rx, res) < 0)
			break;
	}

done:
	res->elapsed_s = (double)(now_ns() - t0) / 1e9;
	res->lost = res->sent > res->received ? res->sent - res->received : 0;
	return 0;
}

static int cmp_u64(const void *a, const void *b)
{
	uint64_t x = *(const uint64_t *)a;
	uint64_t y = *(const uint64_t *)b;

	return (x > y) - (x < y);
}

static void print_row(const char *mode, uint32_t size, struct result *r)
{
	double rate = r->elapsed_s > 0 ? r->received / r->elapsed_s : 0.0;
	double mbs = rate * size / 1e6;

	if (r->rtt_ns && r->received) {
		uint64_t sum = 0;
		uint32_t n = r->received < r->rtt_cap ? r->received : r->rtt_cap;

		qsort(r->rtt_ns, n, sizeof(*r->rtt_ns), cmp_u64);
		for (uint32_t i = 0; i < n; i++)
			sum += r->rtt_ns[i];

		printf("%s,\t%u,\t%u,\t%u,\t%u,\t%u,\t%u,\t%.1f,\t%.1f,\t%.1f,\t%.1f,\t%.1f,\t%.0f,\t%.3f\n",
		       mode, size, r->sent, r->received, r->lost, r->out_of_order, r->corrupt,
		       r->rtt_ns[0] / 1e3, sum / (double)n / 1e3,
		       r->rtt_ns[n / 2] / 1e3, r->rtt_ns[(n * 99) / 100] / 1e3,
		       r->rtt_ns[n - 1] / 1e3, rate, mbs);
	} else {
		printf("%s,\t%u,\t%u,\t%u,\t%u,\t%u,\t%u,\t-\t-\t-\t-\t-\t%.0f,\t%.3f\n",
		       mode, size, r->sent, r->received, r->lost, r->out_of_order, r->corrupt,
		       rate, mbs);
	}
}

static void usage(const char *prog)
{
	fprintf(stderr,
		"usage: %s [-d dev] [-n count] [-w window] [-t timeout_ms]\n"
		"  -d  tty device (default /dev/ttyRPMSG0)\n"
		"  -n  messages per size and mode (default 10000)\n"
		"  -w  throughput window (default 4, M0 has 4 rpmsg RX buffers)\n"
		"  -t  receive timeout in ms (default 1000)\n",
		prog);
}

int main(int argc, char **argv)
{
	struct cfg c = { .dev = "/dev/ttyRPMSG0", .count = 10000, .window = 4, .timeout_ms = 1000 };
	struct termios tio;
	struct result r;
	struct cfg warm;
	int opt;
	int fd;

	while ((opt = getopt(argc, argv, "d:n:w:t:h")) != -1) {
		switch (opt) {
		case 'd': c.dev = optarg; break;
		case 'n': c.count = (uint32_t)strtoul(optarg, NULL, 0); break;
		case 'w': c.window = (uint32_t)strtoul(optarg, NULL, 0); break;
		case 't': c.timeout_ms = atoi(optarg); break;
		default:
			usage(argv[0]);
			return 1;
		}
	}

	if (c.count == 0 || c.window == 0 || c.timeout_ms <= 0) {
		usage(argv[0]);
		return 1;
	}

	fd = open(c.dev, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		fprintf(stderr, "open %s: %s\n", c.dev, strerror(errno));
		return 1;
	}

	if (tcgetattr(fd, &tio) == 0) {
		cfmakeraw(&tio);
		tcsetattr(fd, TCSANOW, &tio);
	}
	tcflush(fd, TCIOFLUSH);

	warm = c;
	warm.count = c.count < WARMUP_COUNT ? c.count : WARMUP_COUNT;
	if (run_latency(fd, &warm, sizes[0], &r) == 0)
		free(r.rtt_ns);

	printf("\tmode,\tsize_B,\tsent,\treceived,\tlost,\tout_of_order,\tcorrupt,\t"
	       "lat_min_us,\tlat_avg_us,\tlat_p50_us,\tlat_p99_us,\tlat_max_us,\tmsgs_per_s,\tMB_per_s\n");

	for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
		if (run_latency(fd, &c, sizes[i], &r) == 0) {
			print_row("latency", sizes[i], &r);
			free(r.rtt_ns);
		}

		if (run_throughput(fd, &c, sizes[i], &r) == 0)
			print_row("throughput", sizes[i], &r);
	}

	close(fd);
	return 0;
}

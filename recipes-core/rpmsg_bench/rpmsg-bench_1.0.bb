SUMMARY = "RPMsg tty latency and throughput benchmark for the RK3506 M0 coprocessor"
DESCRIPTION = "Measures round-trip latency and throughput over /dev/ttyRPMSG* exposed by the rpmsg tty driver."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://rpmsg_bench.c \
    file://Makefile \
"

S = "${UNPACKDIR}"

EXTRA_OEMAKE = "PREFIX=${prefix}"

do_install() {
    oe_runmake DESTDIR=${D} install
}

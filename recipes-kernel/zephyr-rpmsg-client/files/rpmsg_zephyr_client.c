// SPDX-License-Identifier: GPL-2.0-only
/*
 * Remote processor messaging - sample client driver
 *
 * Copyright (C) 2011 Texas Instruments, Inc.
 * Copyright (C) 2011 Google, Inc.
 *
 * Ohad Ben-Cohen <ohad@wizery.com>
 * Brian Swetland <swetland@google.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>

#define MSG_HELLO	"hello"
#define MSG_FETCH	"fetch"

#define RPMSG_CLIENT_NAME	"zephyr-rpmsg-demo"

struct instance_data {
	char message[256];
	struct completion msg_received;
};

static int rpmsg_sample_cb(struct rpmsg_device *rpdev, void *data, int len,
						void *priv, u32 src)
{
	struct instance_data *idata = dev_get_drvdata(&rpdev->dev);

	pr_info("incoming msg: %s\n", (char *)data);
	if (len <= sizeof(idata->message))
		memcpy(idata->message, data, len);
	else
		pr_err("message size %d would overflow (max %d)\n", len, sizeof(idata->message));

	complete(&idata->msg_received);

	return 0;
}

static int find_rpmsg_device(struct device *dev, const void *data)
{
	if (strstr(dev_name(dev), RPMSG_CLIENT_NAME))
		return 1;
	else
		return 0;
}

static ssize_t values_show(struct device_driver *drv, char *buf)
{
	struct device *dev = driver_find_device(drv, NULL, NULL, find_rpmsg_device);
	struct rpmsg_device *rpdev = container_of(dev, struct rpmsg_device, dev);
	struct instance_data *idata;
	ssize_t len;
	int ret;

	if (!rpdev) {
		pr_err("unable to get rpmsg device!\n");
		return -EINVAL;
	}

	idata = dev_get_drvdata(&rpdev->dev);
	reinit_completion(&idata->msg_received);

	/* send a message to our remote processor */
	ret = rpmsg_send(rpdev->ept, MSG_FETCH, strlen(MSG_FETCH));
	if (ret) {
		dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", ret);
		return ret;
	}

	wait_for_completion(&idata->msg_received);

	len = strlen(idata->message);
	memcpy(buf, idata->message, len);

	return len;
}
static DRIVER_ATTR_RO(values);

static struct attribute *rpmsg_imu_attrs[] = {
	&driver_attr_values.attr,
	NULL
};
ATTRIBUTE_GROUPS(rpmsg_imu);

static int rpmsg_sample_probe(struct rpmsg_device *rpdev)
{
	int ret;
	struct instance_data *idata;

	dev_info(&rpdev->dev, "new channel: 0x%x -> 0x%x!\n",
					rpdev->src, rpdev->dst);

	idata = devm_kzalloc(&rpdev->dev, sizeof(*idata), GFP_KERNEL);
	if (!idata)
		return -ENOMEM;

	dev_set_drvdata(&rpdev->dev, idata);
	init_completion(&idata->msg_received);

	/* send a message to our remote processor */
	ret = rpmsg_send(rpdev->ept, MSG_HELLO, strlen(MSG_HELLO));
	if (ret) {
		dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", ret);
		return ret;
	}

	return 0;
}

static void rpmsg_sample_remove(struct rpmsg_device *rpdev)
{
	dev_info(&rpdev->dev, "rpmsg sample client driver is removed\n");
}

static struct rpmsg_device_id rpmsg_driver_sample_id_table[] = {
	{ .name	= RPMSG_CLIENT_NAME },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_driver_sample_id_table);

static struct rpmsg_driver rpmsg_sample_client = {
	.drv.name	= KBUILD_MODNAME,
	.drv.groups	= rpmsg_imu_groups,
	.id_table	= rpmsg_driver_sample_id_table,
	.probe		= rpmsg_sample_probe,
	.callback	= rpmsg_sample_cb,
	.remove		= rpmsg_sample_remove,
};
module_rpmsg_driver(rpmsg_sample_client);

MODULE_DESCRIPTION("Remote processor messaging sample client driver");
MODULE_LICENSE("GPL v2");

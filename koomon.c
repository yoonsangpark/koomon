/*
 * Driver for the Koomonitor
 *
 * Copyright (C) 2023 KOO Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include <linux/gpio.h>
#include <plat/nvt-gpio.h>
#include <linux/interrupt.h>
#include "koomon.h"

#include <linux/ioctl.h>

#define KOO_IOCTL_MAGIC 'K'
#define KOOMON_START	_IOR(KOO_IOCTL_MAGIC, 0x1, int32_t*)
#define KOOMON_STOP	_IOR(KOO_IOCTL_MAGIC, 0x2, int32_t*)

#define MGPIO P_GPIO(11)  /* 11 + 32 = 43 */
//#define P_GPIO(pin)  (pin + 0x20)
static unsigned int irq_num;

#define DRIVER_NAME "koomon"
#define DRIVER_VERSION "0.1"

static irqreturn_t koomon_irq_handler(int irq, void *dev_id) {

        pr_info(">> koomon_irq_handler\n");

        return IRQ_HANDLED;
}

static int koomon_open(struct inode *inode, struct file *file)
{
	pr_info("koomon_open\n");
	return 0;
}

static int koomon_close(struct inode *inodep, struct file *filp)
{
	pr_info("koomon_close\n");
	return 0;
}

static ssize_t koomon_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
	pr_info("koomon_read\n");
	return 0;
}

static ssize_t koomon_write(struct file *file, const char __user *buf,
		       size_t len, loff_t *ppos)
{
	pr_info("koomon_write\n");
	return 0;
}

static void koomon_start(void)
{
        if (request_irq(irq_num,
                  (void *)koomon_irq_handler,
                  IRQF_TRIGGER_FALLING,
                  "koo",
                  NULL)) {

                pr_err("Cannot register interrupt number: %d\n", irq_num);
        }
}

static void koomon_stop(void)
{
	free_irq(irq_num, NULL);
}

static long koomon_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{   
	switch (cmd) {
		case KOOMON_START:
			koomon_start();
			pr_info("KOOMON_START\n");
			break;

		case KOOMON_STOP:
			koomon_stop();
			pr_info("KOOMON_STOP\n");
			break;

		default:
			return 0;
	}

	return 0;
}

static const struct file_operations koomon_misc_fops = {
        .owner		= THIS_MODULE,
        .write		= koomon_write,
        .read		= koomon_read,
        .open		= koomon_open,
        .release	= koomon_close,
	.unlocked_ioctl	= koomon_ioctl,
};

struct miscdevice koomon_misc_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "koomon",
        .fops = &koomon_misc_fops,
};

static int __init misc_init(void)
{
	int ret;

	ret = misc_register(&koomon_misc_device);
	if (ret) {
		pr_err("can't misc_register :(\n");
		return ret;
	}

	ret = gpio_request(MGPIO, "gpio_test");
	if (ret)
        	pr_err("#### failed to request MGPIO\n");

	gpio_direction_input(MGPIO);

	irq_num = gpio_to_irq(MGPIO);
	pr_info("irq_num = %d\n", irq_num);

	pr_info("misc_init : koomon\n");
	return 0;
}

static void __exit misc_exit(void)
{
	misc_deregister(&koomon_misc_device);
	pr_info("misc_exit : koomon\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_NAME);
MODULE_VERSION(DRIVER_VERSION);
MODULE_AUTHOR("yoonsangpark");

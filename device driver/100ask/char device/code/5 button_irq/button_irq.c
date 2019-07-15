#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/gpio.h>

#define DEVICE_NAME "fitz_button"
static int major;

volatile unsigned long *gpx3con = NULL;
volatile unsigned long *gpx3dat = NULL;

static struct class *button_class;
static struct device *button_class_dev;

static irqreturn_t button_irq(int irq, void *dev_id)
{
	printk("irq = %d\n", irq);
	return IRQ_HANDLED;
}

static int button_drv_open(struct inode *inode, struct file *file)
{
	int irq;

	// IRQT_BOTHEDGE
	irq = gpio_to_irq(EXYNOS4_GPX3(2));
	request_irq(irq, button_irq, IRQF_SHARED, "S2", 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(3));
	request_irq(irq, button_irq, IRQF_SHARED, "S3", 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(4));
	request_irq(irq, button_irq, IRQF_SHARED, "S4", 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(5));
	request_irq(irq, button_irq, IRQF_SHARED, "S5", 1);
	return 0;
}

static ssize_t button_drv_read(struct file *file, char __user *buf, size_t size, loff_t *loff)
{
	// 返回4个引脚的电平
	unsigned char keyValue[4];
	int regval;

	if (size != sizeof(keyValue))
		return -EINVAL;

	regval = *gpx3dat;
	keyValue[0] = regval & (1 << 2) ? 1 : 0;
	keyValue[1] = regval & (1 << 3) ? 1 : 0;
	keyValue[2] = regval & (1 << 4) ? 1 : 0;
	keyValue[3] = regval & (1 << 5) ? 1 : 0;

	copy_to_user(buf, keyValue, 4);
	return 4;
}

static int button_drv_release(struct inode *node, struct file *file)
{
	int irq;

	irq = gpio_to_irq(EXYNOS4_GPX3(2));
	free_irq(irq, 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(3));
	free_irq(irq, 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(4));
	free_irq(irq, 1);
	irq = gpio_to_irq(EXYNOS4_GPX3(5));
	free_irq(irq, 1);
}

struct file_operations button_fops = {
	.owner = THIS_MODULE,
	.open = button_drv_open,
	.read = button_drv_read,
	.release = button_drv_release
};

static int __init button_drv_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &button_fops);

	button_class = class_create(THIS_MODULE, "fitz_drv");
	if (IS_ERR(button_class))
		return PTR_ERR(button_class);

	button_class_dev = device_create(button_class, NULL, MKDEV(major, 0), NULL, "fitz_button");
	if (unlikely(IS_ERR(button_class_dev)))
		return PTR_ERR(button_class_dev);

	gpx3con = (unsigned long *)ioremap(0x11000C60, 8);
	gpx3dat = gpx3con + 1;

	return 0;
}

static void __exit button_drv_exit(void)
{
	iounmap(gpx3con);

	device_destroy(button_class, MKDEV(major, 0));
	class_destroy(button_class);

	unregister_chrdev(major, DEVICE_NAME);
}

module_init(button_drv_init);
module_exit(button_drv_exit);

MODULE_LICENSE("GPL");


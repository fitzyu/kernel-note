/*
 * 运行APP 测试驱动 
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/gpio.h>

#define DEVICE_NAME "fitz_button"
static int major;

volatile unsigned long *gpx3con = NULL;
volatile unsigned long *gpx3dat = NULL;

static struct class *button_class;
static struct device *button_class_dev;
static unsigned char key_val;
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);
// 中断时间标志，中断服务程序将它置1，read函数将它清0
static volatile int ev_press = 0;

struct pin_desc {
	unsigned int pin;
	unsigned int keyValue;
};

/*
 * 按下时 0x1 0x2 0x3 0x4
 * 松开时 0x81 0x82 0x83 0x84
 */
struct pin_desc pins_desc[4] = {
	{EXYNOS4_GPX3(2), 0x01},
	{EXYNOS4_GPX3(3), 0x02},
	{EXYNOS4_GPX3(4), 0x03},
	{EXYNOS4_GPX3(5), 0x04},
};

static irqreturn_t button_irq(int irq, void *dev_id)
{
	struct pin_desc *pinDesc = (struct pin_desc *)dev_id;
	printk("irq = %d\n", irq);

	int pinValue = gpio_get_value(pinDesc->pin);
	if (pinValue) { // 松开
		key_val = 0x80 | pinDesc->keyValue;
	} else { // 按下
		key_val = pinDesc->keyValue;
	}

	ev_press = 1; // 表示中断发生了
	wake_up_interruptible(&button_waitq); // 唤醒休眠的进程
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int button_drv_open(struct inode *inode, struct file *file)
{
	int irq;
	int ret;

	// IRQT_BOTHEDGE
	irq = gpio_to_irq(EXYNOS4_GPX3(2));
	ret = request_irq(irq, button_irq, IRQF_SHARED, "S2", (void *)&pins_desc[0]);
	if (ret < 0) {
		return ret;
	}

	irq = gpio_to_irq(EXYNOS4_GPX3(3));
	ret = request_irq(irq, button_irq, IRQF_SHARED, "S3", (void *)&pins_desc[1]);
	if (ret < 0) {
		return ret;
	}
	irq = gpio_to_irq(EXYNOS4_GPX3(4));
	ret = request_irq(irq, button_irq, IRQF_SHARED, "S4", (void *)&pins_desc[2]);
	if (ret < 0) {
		return ret;
	}

	irq = gpio_to_irq(EXYNOS4_GPX3(5));
	ret = request_irq(irq, button_irq, IRQF_SHARED, "S5", (void *)&pins_desc[3]);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

static ssize_t button_drv_read(struct file *file, char __user *buf, size_t size, loff_t *loff)
{
	// 如果没有按键动作发生，就休眠
	wait_event_interruptible(button_waitq, ev_press);
	// 如果有按键动作,返回键值
	copy_to_user(buf, &key_val, 1);
	ev_press = 0;
	return 0;
}

static int button_drv_release(struct inode *node, struct file *file)
{
	int irq;

	irq = gpio_to_irq(EXYNOS4_GPX3(2));
	free_irq(irq, (void *)&pins_desc[0]);
	irq = gpio_to_irq(EXYNOS4_GPX3(3));
	free_irq(irq, (void *)&pins_desc[1]);
	irq = gpio_to_irq(EXYNOS4_GPX3(4));
	free_irq(irq, (void *)&pins_desc[2]);
	irq = gpio_to_irq(EXYNOS4_GPX3(5));
	free_irq(irq, (void *)&pins_desc[3]);

	return 0;
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

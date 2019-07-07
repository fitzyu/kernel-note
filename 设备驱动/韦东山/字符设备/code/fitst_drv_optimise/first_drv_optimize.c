#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>

static struct class *firstdrv_class;
static struct class_device *firstdrv_class_dev;

static int major;

static int first_drv_open(struct inode *inode, struct file *file)
{
	printk("first_drv_open\n");
	return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *user, size_t count, loff_t *offset)
{
	printk("first_drv_write\n");
	return 0;
}

static struct file_operations first_drv_fops = {
	.owner		= THIS_MODULE, // 这是一个宏 推向模块时自动创建的
	.open		= first_drv_open,
	.write	    = first_drv_write, 
};

static int __init fitst_drv_init(void)
{
	// 如果设置0 是系统自动分配，如果不想设置0，就查看/proc/devices 查找空缺的设备号
	major = register_chrdev(0, "first_drv", &first_drv_fops); // 注册驱动程序 告诉内核

	firstdrv_class = class_create(THIS_MODULE, "firstdrv");
	if (IS_ERR(firstdrv_class))
		return PTR_ERR(firstdrv_class);

	firstdrv_class_dev = device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "first_drv");
	if (unlikely(IS_ERR(firstdrv_class_dev)))
		return PTR_ERR(firstdrv_class_dev);
	return 0;
}

static void __exit fitst_drv_exit(void)
{
	device_destroy(firstdrv_class, MKDEV(major, 0));
	class_destroy(firstdrv_class);
	unregister_chrdev(major, "first_drv"); // 卸载
}

module_init(fitst_drv_init);
module_exit(fitst_drv_exit);

MODULE_LICENSE("GPL");


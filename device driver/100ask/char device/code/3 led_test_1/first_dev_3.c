#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>

static struct class *firstdrv_class;
static struct class_device *firstdrv_class_dev;

static int major;

volatile unsigned long *gpm4con = NULL;
volatile unsigned long *gpm4dat = NULL;

static int first_drv_open(struct inode *inode, struct file *file)
{
	
	/*配置GPM4_0 1 2 3为输出*/
	*gpm4con &=~ ((0xf<<(0*4))|(0xf<<(1*4))|(0xf<<(2*4))|(0xf<<(3*4))); // 先清空
	*gpm4con |= ((0x1<<(0*4))|(0x1<<(1*4))|(0x1<<(2*4))|(0x1<<(3*4))); // 配置输出
	
	return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	int val;

	copy_from_user(&val, buf, count);

	if (val == 1) {
		*gpm4dat |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	} else {
		*gpm4dat &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
	}

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
	major = register_chrdev(0, "fitz_led", &first_drv_fops); // 注册驱动程序 告诉内核

	firstdrv_class = class_create(THIS_MODULE, "fitz_drv");
	if (IS_ERR(firstdrv_class))
		return PTR_ERR(firstdrv_class);

	firstdrv_class_dev = device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "fitz_led");
	if (unlikely(IS_ERR(firstdrv_class_dev)))
		return PTR_ERR(firstdrv_class_dev);

	gpm4con = (unsigned int *)ioremap(0x110002E0, 8);
	gpm4dat = gpm4con + 1;
	
	return 0;
}

static void __exit fitst_drv_exit(void)
{
	// 取消映射
	iounmap(gpm4con);

	device_destroy(firstdrv_class, MKDEV(major, 0));
	class_destroy(firstdrv_class);
	unregister_chrdev(major, "fitz_led"); // 卸载
}

module_init(fitst_drv_init);
module_exit(fitst_drv_exit);

MODULE_LICENSE("GPL");


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
	
	/*����GPM4_0 1 2 3Ϊ���*/
	*gpm4con &=~ ((0xf<<(0*4))|(0xf<<(1*4))|(0xf<<(2*4))|(0xf<<(3*4))); // �����
	*gpm4con |= ((0x1<<(0*4))|(0x1<<(1*4))|(0x1<<(2*4))|(0x1<<(3*4))); // �������
	
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
	.owner		= THIS_MODULE, // ����һ���� ����ģ��ʱ�Զ�������
	.open		= first_drv_open,
	.write	    = first_drv_write, 
};

static int __init fitst_drv_init(void)
{
	// �������0 ��ϵͳ�Զ����䣬�����������0���Ͳ鿴/proc/devices ���ҿ�ȱ���豸��
	major = register_chrdev(0, "fitz_led", &first_drv_fops); // ע���������� �����ں�

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
	// ȡ��ӳ��
	iounmap(gpm4con);

	device_destroy(firstdrv_class, MKDEV(major, 0));
	class_destroy(firstdrv_class);
	unregister_chrdev(major, "fitz_led"); // ж��
}

module_init(fitst_drv_init);
module_exit(fitst_drv_exit);

MODULE_LICENSE("GPL");


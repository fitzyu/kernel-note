#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

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
	.owner		= THIS_MODULE, // ����һ���� ����ģ��ʱ�Զ�������
	.open		= first_drv_open,
	.write	    = first_drv_write, 
};

static int __init fitst_drv_init(void)
{
	// �������0 ��ϵͳ�Զ����䣬�����������0���Ͳ鿴/proc/devices ���ҿ�ȱ���豸��
	major = register_chrdev(0, "first_drv", &first_drv_fops); // ע���������� �����ں�
	return 0;
}

static void __exit fitst_drv_exit(void)
{
	unregister_chrdev(major, "first_drv"); // ж��
}

module_init(fitst_drv_init);
module_exit(fitst_drv_exit);

MODULE_LICENSE("GPL");



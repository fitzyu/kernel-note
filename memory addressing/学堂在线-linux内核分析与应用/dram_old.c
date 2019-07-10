#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/uaccess.h>

char modName = "dram";
int deviceMajor = 85;
loff_t dramSize;

static DramRead(struct file *file, char *buf, size_t count, loff_t *pos)
{
	struct page *pp;
	void *from;
	int pageNumber, pageIndent, more;

	if (*pos > dramSize) {
		return 0;
	}

	pageNumber = *pos / PAGE_SIZE;
	pageIndent = *pos % PAGE_SIZE;

	pp = &mem_map[pageNumber];
	from = kmap(pp) + pageIndent;

	if ((pageIndent + count) > PAGE_SIZE) {
		count = PAGE_SIZE - pageIndent;
	}

	more = copy_to_user(buf, from, count);
	if (more) {
		return -EFAULT;
	}

	kunmap(pp);

	*pos += count;
	return count;
}

loff_t Dramllseek(struct file *file, loff_t offset, int whence)
{
	loff_t newPos = -1;

	switch (whence) {
		case 0: // SEEK_SET
			newPos = offset;
			break;
		case 1: // SEEK_CUR
			newPos = file->f_pos + offset;
			break;
		case 2: // SEEK_END
			newPos = dramSize + offset;
			break;
	}

	if ((newPos < 0) || (newPos > dramSize)) {
		return -EINVAL;
	}

	file->f_pos = newPos;
	return newPos;
}

struct file_operations DramFops = {
	.owner = THIS_MODULE,
	.llseek = Dramllseek,
	.read = DramRead
};

static int __init DramInit(void)
{
	printk("Install %s module\n", modName);
	printk("deviceMajor = %d\n", deviceMajor);
	dramSize = (loff_t)numPhyspages << PAGE_SHIFT;

	printk("ramtop = %08llX (%llu MB)\n", dramSize, dramSize >> 20);

	return register_chrdev(deviceMajor, modName, &DramFops);
}

static void __exit DramExit(void)
{
	unregister_chrdev(deviceMajor, modName);
	printk("removing %s module\n", modName);
}

MODULE_LICENSE("GPL");
module_init(DramInit);
module_exit(DramExit);

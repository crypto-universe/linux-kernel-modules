#include <linux/module.h>
#include <linux/fs.h>	//file system
#include <linux/uaccess.h>	// user space -> kernel space
#include <linux/init.h>
#include <linux/slab.h>	// PAGE_SIZE
#include <linux/cdev.h>	// char devices

static char* kbuf;
static dev_t first;	//device id
static unsigned int count = 1;
static int MAJOR = 700;	//char dev
static int MINOR = 0;
static struct cdev* my_cdev;

#define DEVICE_NAME "Dummy"
#define KBUF_SIZE ((size_t) ((10) * PAGE_SIZE))

static int mychardev_open(struct inode* inode, struct file* file) {
	printk(KERN_INFO "Opening device %s.\n", DEVICE_NAME);
	return 0;
}

static int mychardev_release(struct inode* inode, struct file* file) {
	printk(KERN_INFO "Releasing device %s.\n", DEVICE_NAME);
	return 0;
}

static ssize_t mychardev_read(struct file* file, char __user* buf, size_t lbuf, loff_t *ppos) {
	const unsigned int num_of_bytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);
	*ppos += num_of_bytes;
	
	printk(KERN_INFO "Reading device: %s; number of bytes: %u; position: %u.\n", DEVICE_NAME, num_of_bytes, (unsigned int)*ppos);
	return num_of_bytes;
}

static ssize_t mychardev_write(struct file* file, const char __user* buf, size_t lbuf, loff_t *ppos) {
	const unsigned int num_of_bytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += num_of_bytes;
	
	printk(KERN_INFO "Writing device: %s; number of bytes: %u; position: %u.\n", DEVICE_NAME, num_of_bytes, (unsigned int)*ppos);
	return num_of_bytes;
}

static const struct file_operations mycdev_fops = {
	.owner = THIS_MODULE,
	.read = mychardev_read,
	.write = mychardev_write,
	.open = mychardev_open,
	.release = mychardev_release
};

int init_module(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
	// check errors

	first = MKDEV(MAJOR, MINOR);
	register_chrdev_region(first, count, DEVICE_NAME);
	// check errors

	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdev_fops);
	cdev_add(my_cdev, first, count);

	return 0;
}

void cleanup_module(void) {
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

MODULE_LICENSE("MIT");
MODULE_AUTHOR("crypto-universe")
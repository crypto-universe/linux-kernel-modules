#include <linux/module.h>
#include <linux/fs.h>	//file system
#include <linux/uaccess.h>	// user space -> kernel space
#include <linux/init.h>
#include <linux/slab.h>	// PAGE_SIZE
#include <linux/cdev.h>	// char devices

static dev_t first;	//device id
static unsigned int count = 1;
static int MAJOR = 700;	//char dev
static int MINOR = 0;
static struct cdev* my_cdev;

#define DEVICE_NAME "Dummy"
#define KBUF_SIZE ((size_t) ((10) * PAGE_SIZE))

static int mychardev_open(struct inode* inode, struct file* file) {
	static unsigned int counter = 0;	// resource number

	char* kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
	file->private_data = kbuf;

	++counter;
	
	printk(KERN_INFO "Opening device %s. Counter = %u. Module refcounter = %u.\n",
		DEVICE_NAME, counter, module_refcount(THIS_MODULE));

	return 0;
}

static int mychardev_release(struct inode* inode, struct file* file) {
	char* kbuf = file->private_data;
	
	if (kbuf != NULL) {
		printk("Free buffer.");
		kfree(kbuf);
		kbuf = NULL;
		file->private_data = NULL;
	}
	
	printk(KERN_INFO "Releasing device %s.\n", DEVICE_NAME);
	return 0;
}

static ssize_t mychardev_read(struct file* file, char __user* buf, size_t lbuf, loff_t *ppos) {
	char* kbuf = file->private_data;

	const unsigned int num_of_bytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);
	*ppos += num_of_bytes;
	
	printk(KERN_INFO "Reading device: %s; number of bytes: %u; position: %u.\n", DEVICE_NAME, num_of_bytes, (unsigned int)*ppos);
	return num_of_bytes;
}

static ssize_t mychardev_write(struct file* file, const char __user* buf, size_t lbuf, loff_t *ppos) {
	char* kbuf = file->private_data;

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

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);

	first = MKDEV(MAJOR, MINOR);
	register_chrdev_region(first, count, DEVICE_NAME);
	// check errors

	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdev_fops);
	cdev_add(my_cdev, first, count);

	return 0;
}

static void __exit cleanup_device(void) {
	if (my_cdev != NULL) {
		cdev_del(my_cdev);
	}
	unregister_chrdev_region(first, count);
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("crypto-universe")
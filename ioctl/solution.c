#include <linux/module.h>
#include <linux/fs.h>	//file system
#include <linux/uaccess.h>	// user space -> kernel space
#include <linux/init.h>
#include <linux/slab.h>	// PAGE_SIZE
#include <linux/cdev.h>	// char devices
#include <linux/kernel.h>	// simple_strtol

static dev_t first;	//device id
static unsigned int M = 0, N = 0;	// counters
static const int MAJOR = 240;	//char dev
static const int MINOR = 0;
static const int NUM_OF_MINORS = 1;
static struct cdev* my_cdev;

#define DEVICE_NAME "Dummy"

#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	const char __user * str = (const char __user *) arg;
	long result = 0;
	char *endp = NULL;

	switch(cmd) {
	case SUM_LENGTH:
		M += strlen(str);
		result = M;
		break;
	
	case SUM_CONTENT:
		N += simple_strtol(str, &endp, 10);
		result = N;
		break;
	
	default:
		printk(KERN_INFO "Unsupported command %u with argument %lu. Skip it.",
				cmd, arg);
	}
	
	return result;
}

static int mychardev_open(struct inode* inode, struct file* file) {
	char* kbuf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	file->private_data = kbuf;
	
	// There is some data to read
	*((bool*)kbuf) = true;
	
	printk(KERN_INFO "Opening device %s. Module refcounter = %u.\n",
		DEVICE_NAME, module_refcount(THIS_MODULE));

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
	char local_buf[20];
	bool* isDataToPrint = ((bool*)file->private_data);

	if (*isDataToPrint == true) {
		*isDataToPrint = false;
		const unsigned int num_of_bytes = sprintf(local_buf, "%u %u\n", M, N);
		const unsigned int num_of_bytes_copied = num_of_bytes - copy_to_user(buf, local_buf, num_of_bytes);	
	
		printk(KERN_INFO "Reading device: %s; copied: %u, data: %s",
			DEVICE_NAME, num_of_bytes_copied, local_buf);
		return num_of_bytes_copied;
	} else {
		return 0;
	}
}

static const struct file_operations mycdev_fops = {
	.owner = THIS_MODULE,
	.read = mychardev_read,
	.open = mychardev_open,
	.release = mychardev_release,
	.unlocked_ioctl = device_ioctl,
};

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);

	first = MKDEV(MAJOR, MINOR);
	register_chrdev_region(first, NUM_OF_MINORS, DEVICE_NAME);
	// check errors

	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdev_fops);
	cdev_add(my_cdev, first, NUM_OF_MINORS);

	return 0;
}

static void __exit cleanup_device(void) {
	if (my_cdev != NULL) {
		cdev_del(my_cdev);
	}
	unregister_chrdev_region(first, NUM_OF_MINORS);
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
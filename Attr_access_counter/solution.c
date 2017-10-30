#include <linux/module.h>

static const char* KOBJ_NAME = "my_kobject";
static struct kobject* device_kobject;
static unsigned int my_sys = 0;

#define DEVICE_NAME "Dummy"

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	++my_sys;
	return sprintf(buf, "%d\n", my_sys);
}

static const struct kobj_attribute my_sys_attr = __ATTR_RO(my_sys);

static int __init init_device(void) {
	int result;
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);

	device_kobject = kobject_create_and_add(KOBJ_NAME, kernel_kobj);
	if (device_kobject == NULL)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	result = sysfs_create_file(device_kobject, &my_sys_attr.attr);

	if (result != 0)
		kobject_put(device_kobject);

	return result;
}

static void __exit cleanup_device(void) {
	kobject_put(device_kobject);

	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("crypto-universe")
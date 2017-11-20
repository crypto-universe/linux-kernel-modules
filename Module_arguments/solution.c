#include <linux/module.h>

static const char* KOBJ_NAME = "my_kobject";
static struct kobject* device_kobject;

static int a = 0;
static int b = 0;
static int c[5] = {0, 0, 0, 0, 0};
static int arr_argc = 0;

#define DEVICE_NAME "Dummy"

module_param(a, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(a, "Integer a");
module_param(b, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(b, "Integer b");
module_param_array(c, int, &arr_argc, 0000);
MODULE_PARM_DESC(c, "An array of 5 integers");

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int i = 0;
	int result = a + b;
	for (i=0; i<arr_argc; i++) {
		result += c[i];
	}
	return sprintf(buf, "%d\n", result);
}

static const struct kobj_attribute my_sys_attr = __ATTR_RO(my_sys);

static int __init init_device(void) {
	int result;
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	printk(KERN_INFO "a = %d, b = %d\n", a, b);
	printk(KERN_INFO "c[0] = %d, c[1] = %d, c[2] = %d, c[3] = %d, c[4] = %d\n",
		c[0], c[1], c[2], c[3], c[4]);
	printk(KERN_INFO "Array size = %d\n", arr_argc);

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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
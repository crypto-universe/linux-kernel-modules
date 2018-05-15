#include <linux/module.h>
#include <linux/device.h>   //device struct
#include <linux/slab.h>     //kmalloc
#include "checker.h"

static void* raw_ptr;
static int* int_array_ptr;
static struct device* device_ptr;

static int __init init_my_module(void) {
	printk(KERN_INFO "Hello from Dummy module ;)\n");

	ssize_t size_raw_ptr = get_void_size();
	raw_ptr = kmalloc(size_raw_ptr, GFP_KERNEL);
	submit_void_ptr(raw_ptr);

	ssize_t size_int_ptr = get_int_array_size();
	int_array_ptr = kmalloc(size_int_ptr * sizeof(int), GFP_KERNEL);
	submit_int_array_ptr(int_array_ptr);

	device_ptr = kmalloc(sizeof(struct device), GFP_KERNEL);
	submit_struct_ptr(device_ptr);
	return 0;
}

static void __exit cleanup_my_module(void) {
	checker_kfree(device_ptr);
	checker_kfree(int_array_ptr);
	checker_kfree(raw_ptr);
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_my_module);
module_exit (cleanup_my_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
#include <linux/module.h>

int init_module(void) {
	printk(KERN_INFO "Hello from Dummy module ;)\n");
	return 0;
}

void cleanup_module(void) {
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

MODULE_LICENSE("MIT");
MODULE_AUTHOR("crypto-universe")
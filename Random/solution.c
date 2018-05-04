#include <linux/module.h>
#include <linux/init.h>
#include <linux/random.h>
#include "checker.h"

#define DEVICE_NAME "Dummy_logger"

int reference_sum_counter(int *arr, size_t n) {
	int result = 0, i = 0;
	for(i=0; i<n; i++) {
		result += arr[i];
	}
	return result;
}

unsigned int get_positive_int(unsigned int argument) {
	unsigned int init_random = get_random_int();
	return 10 + init_random % argument;
}

void comparer(void) {
	unsigned int iterations = get_positive_int(15);
	u8 array[30*sizeof(int)];
	char buf[500];
	
	u32 i = 0;
	for(i=0; i<iterations; i++) {
		unsigned int len = get_positive_int(20);
		get_random_bytes(array, len*sizeof(int));
		int sum1 = array_sum((int*)array, len);
		int sum2 = reference_sum_counter((int*)array, len);
		generate_output(sum1, (int*)array, len, buf);
		if(sum1 == sum2) {
			printk(KERN_INFO "%s\n", buf);
		} else {
			printk(KERN_ERR "%s\n", buf);
		}
	}
}

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	CHECKER_MACRO;
	comparer();
	return 0;
}

static void __exit exit_device(void) {
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
	CHECKER_MACRO;
}

module_init (init_device);
module_exit (exit_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
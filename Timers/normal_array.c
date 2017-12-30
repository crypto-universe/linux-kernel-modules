#include <linux/module.h>
#include <linux/timer.h>

#define NUM_OF_TIMERS 20

static struct timer_list timers[NUM_OF_TIMERS];
static unsigned long delays[NUM_OF_TIMERS];
static int arr_len = 0;

static unsigned long last_jiffy = 0;

#define DEVICE_NAME "Dummy"

module_param_array(delays, long, &arr_len, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(delays, "Array of timer delays.");

static void timer_handler(unsigned long data) {
	const unsigned long now = jiffies;
	printk(KERN_INFO "Timer %lu was called at %lu, difference is %u!",
			data, now, jiffies_to_msecs(now-last_jiffy));
	last_jiffy = now;
}

static void my_init_timers(void) {
	int i = 0;
	unsigned long start = jiffies + HZ;
	last_jiffy = start;

	for (i=0; i<arr_len; ++i) {
		const unsigned long delay = delays[i];
		const unsigned long expire_time = start + msecs_to_jiffies(delay);
	
		init_timer_on_stack(&timers[i]);

		timers[i].expires = expire_time;
		timers[i].function = &timer_handler;
		timers[i].data = delay;
		
		add_timer(&timers[i]);
		start = expire_time;
	}
}

static void delete_timers(void) {
	int i = 0;

	for (i=0; i<arr_len; ++i) {
		del_timer(&timers[i]);
	}
}

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	my_init_timers();
	return 0;
}

static void __exit cleanup_device(void) {
	delete_timers();
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
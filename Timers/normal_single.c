#include <linux/module.h>
#include <linux/timer.h>

#define NUM_OF_TIMERS 20

static struct timer_list my_timer;
static unsigned long delays[NUM_OF_TIMERS];
static int arr_len = 0;
static unsigned int delays_index = 0;

static unsigned long last_jiffy = 0;

#define DEVICE_NAME "Dummy"

module_param_array(delays, long, &arr_len, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(delays, "Array of timer delays.");

static void timer_handler(unsigned long data) {
	const unsigned long now = jiffies;
	printk(KERN_INFO "Timer %lu was called at %lu, difference is %u!",
			data, now, jiffies_to_msecs(now-last_jiffy));
	last_jiffy = now;
	
	const unsigned long delay = delays[delays_index++];
	if (delay == 0) {
		const unsigned long expire_time = now + msecs_to_jiffies(delay);
		my_timer.expires = expire_time;
		my_timer.data = delay;
	
		add_timer(&my_timer);
	}
}

static void my_init_timers(void) {
	unsigned long start = jiffies;
	last_jiffy = start;

	const unsigned long delay = delays[delays_index++];
	const unsigned long expire_time = start + msecs_to_jiffies(delay);

	init_timer_on_stack(&my_timer);

	my_timer.expires = expire_time;
	my_timer.function = &timer_handler;
	my_timer.data = delay;
	
	add_timer(&my_timer);
}

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	my_init_timers();
	return 0;
}

static void __exit cleanup_device(void) {
	del_timer(&my_timer);
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
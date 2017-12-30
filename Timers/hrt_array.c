#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>	// HRTIMER_NORESTART

#define NUM_OF_TIMERS 20

static struct hrtimer timers[NUM_OF_TIMERS];
static unsigned long delays[NUM_OF_TIMERS];
static int arr_len = 0;

static unsigned long last_jiffy = 0;

#define DEVICE_NAME "Dummy"

module_param_array(delays, long, &arr_len, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(delays, "Array of timer delays.");

static enum hrtimer_restart timer_handler(struct hrtimer* t) {
	const unsigned long now = jiffies;
	printk(KERN_INFO "Timer was called at %lu, difference is %u!",
			now, jiffies_to_msecs(now-last_jiffy));
	last_jiffy = now;
	return HRTIMER_NORESTART;
}

static void my_init_timers(void) {
	int i = 0;
	long delay_accum = 0;
	last_jiffy = jiffies;

	for (i=0; i<arr_len; ++i) {
		const unsigned long delay = delays[i];
		struct hrtimer* current_hrtimer_ptr = &timers[i];

		delay_accum += delay;

		hrtimer_init_on_stack(current_hrtimer_ptr, CLOCK_REALTIME, HRTIMER_MODE_REL);
		current_hrtimer_ptr->function = &timer_handler;
		hrtimer_start(current_hrtimer_ptr, ms_to_ktime(delay_accum), HRTIMER_MODE_REL);
	}
}

static void delete_timers(void) {
	int i = 0;

	for (i=0; i<arr_len; ++i) {
		hrtimer_cancel(&timers[i]);
		destroy_hrtimer_on_stack(&timers[i]);
	}
}

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	my_init_timers();
	check_timer();
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
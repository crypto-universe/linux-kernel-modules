#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>	// HRTIMER_NORESTART

#define MAX_NUM_OF_TIMERS 20

static struct hrtimer timer;

static unsigned long delays[MAX_NUM_OF_TIMERS];
static int delays_len = 0;
static int delays_index = 0;

static unsigned long last_jiffy = 0;

#define DEVICE_NAME "Dummy"

module_param_array(delays, long, &delays_len, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(delays, "Array of timer delays.");

static void timer_log(void) {
	const unsigned long now = jiffies;
	printk(KERN_INFO "Timer was called at %lu, difference is %u!",
			now, jiffies_to_msecs(now-last_jiffy));
	last_jiffy = now;
}

static enum hrtimer_restart timer_handler(struct hrtimer* t) {
	const long delay_as_long = delays[delays_index];
	timer_log();

	if (++delays_index < delays_len) {
		// I'm not sure this is correct. But it works.
		hrtimer_start(&timer, ms_to_ktime(delay_as_long), HRTIMER_MODE_REL);
		return HRTIMER_RESTART;
	} else {
		return HRTIMER_NORESTART;
	}
}

static inline void init_my_timer(void) {
	hrtimer_init(&timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	timer.function = &timer_handler;
	hrtimer_start(&timer, ms_to_ktime(delays[delays_index]), HRTIMER_MODE_REL);
	++delays_index;
}

static int __init init_device(void) {
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);
	init_my_timer();
	return 0;
}

static void __exit cleanup_device(void) {
	hrtimer_cancel(&timer);
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
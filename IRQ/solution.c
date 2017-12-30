#include <linux/module.h>
#include <linux/interrupt.h>

static const int IRQ = 8;
static const char* KOBJ_NAME = "my_kobject";
static struct kobject* device_kobject;
static void* dev_id;

static unsigned long interrupts_counter;

#define DEVICE_NAME "Dummy"

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	ssize_t result = sprintf(buf, "%lu\n", interrupts_counter);
	return result;
}

static irqreturn_t irq_handler(int irq, void* dev_id) {
	printk(KERN_INFO "IRQ %d interrupt.\n", irq);
	if (irq == IRQ) {
		++interrupts_counter;
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static const struct kobj_attribute my_sys_attr = __ATTR_RO(my_sys);

static int __init init_device(void) {
	int result;
	interrupts_counter = 0;
	
	printk(KERN_INFO "Hello from %s ;)\n", DEVICE_NAME);

	device_kobject = kobject_create_and_add(KOBJ_NAME, kernel_kobj);
	if (device_kobject == NULL)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	result = sysfs_create_file(device_kobject, &my_sys_attr.attr);

	if (result != 0)
		kobject_put(device_kobject);
	
	result = request_irq(IRQ, &irq_handler, IRQF_TIMER | IRQF_SHARED, DEVICE_NAME, THIS_MODULE);
	if (result != 0) {
		printk(KERN_INFO "IRQ request failed with code %d.\n", result);
		// If returned result != 0, module won't be loaded, but kobject is already created.
		// Let's make a small cleanup.
		kobject_put(device_kobject);
	}
	
	return result;
}

static void __exit cleanup_device(void) {
	kobject_put(device_kobject);
	if (dev_id != NULL) {
		free_irq(IRQ, dev_id);
	}
	printk(KERN_INFO "OK, OK, leaving kernel.\n");
}

module_init (init_device);
module_exit (cleanup_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
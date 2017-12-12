#include <linux/module.h>
#include <linux/list.h>

static const char* KOBJ_NAME = "my_kobject";
static struct kobject* device_kobject;

#define DEVICE_NAME "Dummy"

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	// This module was loaded just now, so it is first in the module list.
	// Getting prev element gives us real list head.
	struct list_head* original_list_head = (THIS_MODULE->list).prev;

	unsigned int i = 0;
	unsigned int total_len_counter = 0;
	unsigned int insertion_index = 0;
	unsigned int mod_names_counter = 0;
	char* mod_names[100];
	struct module* tmp = NULL;
	char* curr_mod_name = NULL;
	struct list_head* current_list_element = NULL;
	
	list_for_each(current_list_element, original_list_head) {
		tmp = list_entry(current_list_element, struct module, list);
		curr_mod_name = tmp->name;
		
		insertion_index = mod_names_counter;
		while(insertion_index > 0
				&& strncmp(mod_names[insertion_index-1], curr_mod_name, MODULE_NAME_LEN) > 0) {
			mod_names[insertion_index] = mod_names[insertion_index-1];
			--insertion_index;
		}
		mod_names[insertion_index] = curr_mod_name;
		++mod_names_counter;
	}
	
	for(i=0; i < mod_names_counter; i++) {
		total_len_counter += sprintf(buf+total_len_counter, "%s\n", mod_names[i]);
	}
	return total_len_counter;
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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("crypto-universe");
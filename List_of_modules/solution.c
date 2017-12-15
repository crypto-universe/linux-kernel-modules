#include <linux/module.h>
#include <linux/list.h>

static const char* KOBJ_NAME = "my_kobject";
static struct kobject* device_kobject;

#define DEVICE_NAME "Dummy"

static void insert_in_sorted_arr(const unsigned int arr_size,
		const char** str_arr, const char* str_to_add) {
	unsigned int insertion_index = arr_size;
	while(insertion_index > 0
			&& strncmp(str_arr[insertion_index-1], str_to_add, MODULE_NAME_LEN) > 0) {
		str_arr[insertion_index] = str_arr[insertion_index-1];
		--insertion_index;
	}
	str_arr[insertion_index] = str_to_add;
}

static ssize_t print_str_arr_to_buffer(const unsigned int arr_size,
		const char** arr_to_print, char* dst_buf) {
	unsigned int i = 0;
	ssize_t total_len_counter = 0;
	for(i = 0; i < arr_size; i++) {
		total_len_counter += sprintf(dst_buf+total_len_counter, "%s\n", arr_to_print[i]);
	}
	return total_len_counter;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	// This module was loaded just now, so it is first in the module list.
	// Getting prev element gives us real list head.
	struct list_head* original_list_head = (THIS_MODULE->list).prev;

	unsigned int mod_names_counter = 0;
	const char* mod_names[100];
	struct module* tmp = NULL;
	struct list_head* current_list_element = NULL;
	
	list_for_each(current_list_element, original_list_head) {
		tmp = list_entry(current_list_element, struct module, list);
		insert_in_sorted_arr(mod_names_counter, mod_names, tmp->name);
		++mod_names_counter;
	}
	
	return print_str_arr_to_buffer(mod_names_counter, mod_names, buf);
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
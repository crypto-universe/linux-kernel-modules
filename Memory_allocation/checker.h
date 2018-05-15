ssize_t get_void_size(void) {
	return 10;
}

ssize_t get_int_array_size(void) {
	return 5;
}

void submit_void_ptr(void *p) { }
void submit_int_array_ptr(int *p) { }
void submit_struct_ptr(struct device *p) { }

void checker_kfree(void *p) {
	printk(KERN_INFO "Free pointer %p.", p);
	kfree(p);
}
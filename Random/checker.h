#define CHECKER_MACRO printk("CHECKER_MACRO");

int array_sum(int *arr, size_t n) {
	int result = 0, i = 0;
	for(i=0; i<n; i++) {
		result += arr[i];
	}
	return result;
}

ssize_t generate_output(int sum, int *arr, size_t size, char *buf) {
	printk(KERN_INFO "%d\n", sum);
	return 0;
}
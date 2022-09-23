#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(abhishekhello){
	printk("Hello from first question\n");
	return 0;
}

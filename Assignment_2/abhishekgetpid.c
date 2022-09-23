#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/cred.h>

SYSCALL_DEFINE0(abhishekgetpid){
	printk("Demo of my system call using predefined syscall,  predefined getpid syscall returned - %d", task_tgid_vnr(current));
	return task_tgid_vnr(current);
}

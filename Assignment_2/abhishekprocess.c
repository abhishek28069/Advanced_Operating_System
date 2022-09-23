#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/cred.h>

SYSCALL_DEFINE0(abhishekprocess){
	struct task_struct *parent = current->parent;
	printk("Your process id is - %d, YOur parent id is - %d", current->pid, parent->pid);
	return 0;
}

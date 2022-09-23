#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE1(abhishekprint, char *, message){
	char buffer[256];
	long copycheck = strncpy_from_user(buffer, message, sizeof(buffer));
	if(copycheck < 0 || copycheck == sizeof(buffer))
		return -EFAULT;
	printk(KERN_INFO "abhishekprint system-call called with \"%s\"\n",buffer);
	return 0;
}

#include "core.h"
#include "util.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
#include <linux/sched/signal.h>
#else
#include <linux/sched.h>
#endif

static DEFINE_MUTEX(module_mutex);
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */

void analyze_fops(void) {
    unsigned long addr;
	const char *mod_name;
	struct file *fp;
	struct module *mod;

	printk("[sys_inspector.ko] Analyzing /proc File Operations...\n");

	fp = filp_open("/proc", O_RDONLY, S_IRUSR);
	if (IS_ERR(fp)){
		printk("[sys_inspector] Failed to open /proc.");
		return;
	}

	if (IS_ERR(fp->f_op)){
		printk("[sys_inspector] /proc has no fops.");
		return;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	addr = (unsigned long)fp->f_op->iterate;
#else
	addr = (unsigned long)fp->f_op->readdir;
#endif

	if (!ckt(addr)){
		mutex_lock(&module_mutex);
		mod = get_module_from_addr(addr);
		if (mod){
			printk("[sys_inspector] Module [%s] hijacked /proc fops.\n", mod->name);
		} else {
			mod_name = find_hidden_module(addr);
			if (mod_name){
				printk("[sys_inspector] Module [%s] hijacked /proc fops.\n", mod_name);
			}
		}
		mutex_unlock(&module_mutex);
	}
}

void analyze_procs(void) {
    analyze_fops();
}
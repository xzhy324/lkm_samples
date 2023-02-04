#include "core.h"
#include "util.h"
#include "syscall_table.h"

unsigned long *sct = NULL; /* Syscall Table */
int (*ckt)(unsigned long addr) = NULL; /* Core Kernel Text */

static void execute_analysis(void) {
    printk("executing analysis!");
    analyze_syscalls();
}

static int init_kernel_syms(void){
	sct = (void *)lookup_name("sys_call_table");
	ckt = (void *)lookup_name("core_kernel_text");

	if (!sct || !ckt)
		return -1;

	return 0;
}

static int lkm_init(void)
{
    printk("================================");
    printk("Sys_inspector: loaded!\n");
    if (init_kernel_syms() < 0){
        printk("ERROR: Failed to lookup symbols\n");
        return -1;
    }
    printk("sct: %p",sct);
    printk("core_text: %p",ckt);
    execute_analysis();
    return 0;
}
 
static void lkm_exit(void)
{
    printk("Sys_inspector: removed!\n");
    printk("================================");
}
 
module_init(lkm_init);
module_exit(lkm_exit);
MODULE_LICENSE("GPL");
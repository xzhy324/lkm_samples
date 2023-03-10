#include "core.h"
#include "util.h"
#include "syscall_table.h"
#include "idt_table.h"
#include "module_list.h"

unsigned long *sct = NULL; /* Syscall Table */
unsigned long *idt = NULL; /* IDT Table*/
int (*ckt)(unsigned long addr) = NULL; /* Core Kernel Text */

static void execute_analysis(void) {
    //printk("executing analysis!");
    analyze_syscalls();
    //analyze_idt();
    analyze_modules();
}

static int init_kernel_syms(void){
	sct = (void *)lookup_name("sys_call_table");
    idt = (void *)lookup_name("idt_table");
	ckt = (void *)lookup_name("core_kernel_text");

	if (!sct || !ckt || !idt)
		return -1;

	return 0;
}

static int lkm_init(void)
{
    printk("================================");
    printk("[sys_inspector.ko] Sys_inspector: loaded!\n");
    if (init_kernel_syms() < 0){
        printk("ERROR: Failed to lookup symbols\n");
        return -1;
    }
    printk("[sys_inspector.ko] idt_table: %p",idt);
    printk("[sys_inspector.ko] sys_call_table: %p",sct);
    printk("[sys_inspector.ko] core_kernel_text(addr): %p",ckt);
    execute_analysis();
    return 0;
}
 
static void lkm_exit(void)
{
    printk("[sys_inspector.ko] Sys_inspector: removed!\n");
    printk("================================");
}
 
module_init(lkm_init);
module_exit(lkm_exit);
MODULE_LICENSE("GPL");
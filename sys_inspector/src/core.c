#include "core.h"
#include "util.h"
#include "syscall_table.h"
#include "idt_table.h"
#include "module_list.h"
#include "network_port.h"
#include "proc.h"
#include "netfilter.h"

unsigned long *sct = NULL; /* Syscall Table */
unsigned long *idt = NULL; /* IDT Table*/
int (*ckt)(unsigned long addr) = NULL; /* Core Kernel Text */
unsigned long kernel_base = NULL;

static void execute_analysis(void) {
    //printk("executing analysis!");
    analyze_syscalls();
    analyze_idt();
    analyze_modules();
    analyze_networks();
    analyze_procs();
    analyze_netfilter();
}

static int init_kernel_syms(void){
	sct = (void *)lookup_name("sys_call_table");
    idt = (void *)lookup_name("idt_table");
	ckt = (void *)lookup_name("core_kernel_text");
    kernel_base = lookup_name("startup_64");

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
    printk("[sys_inspector.ko] idt_table: %lx",idt);
    printk("[sys_inspector.ko] sys_call_table: %lx",sct);
    printk("[sys_inspector.ko] core_kernel_text(addr): %lx",ckt);
    printk("[sys_inspector.ko] kernel_base: %lx",kernel_base);
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
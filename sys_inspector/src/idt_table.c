#include "core.h"
#include "util.h"

#include <linux/interrupt.h>
#include <asm/processor.h>
#include <linux/mm.h>
#include <asm-generic/io.h>

static DEFINE_MUTEX(module_mutex);

extern unsigned long *idt; /* IDT Table */
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */

#define IDT_NUM 256


void analyze_idt(void){
	int i;
    unsigned long addr;
    struct module *mod;
    
    printk("[sys_inspector.ko] Analyzing idt_table...");
    for (i=0; i< IDT_NUM; i++){
        addr = idt[i];//x86架构下，idt表中是中断描述符（门描述符一种）
        if (!ckt(addr)) {
			//printk(KERN_ALERT"[sys_inspector.ko] idt[%d] addr:%lx lays in ring3!", i, addr);
            // mutex_lock(&module_mutex);
            // mod = get_module_from_addr(addr);
            // mutex_unlock(&module_mutex);
            // printk(KERN_ALERT"[sys_inspector.ko] Module [%s] is hooking", mod->name);
		}
    }

}
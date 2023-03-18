#include <asm/asm-offsets.h> /* NR_syscalls */

#include "core.h"
#include "util.h"


extern unsigned long *sct; /* Syscall Table */
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */



void analyze_syscalls(void){
	int i;
    unsigned long addr;
    printk("[sys_inspector.ko] analyzing syscall!");

    for (i=0; i< NR_syscalls; i++){
        addr = sct[i];
        if (!ckt(addr)) {
			printk(KERN_ALERT"[sys_inspector.ko] syscall[%d] addr:%lx lays in ring3!", i, addr);
		}
    }

}
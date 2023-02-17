// #include <asm/asm-offsets.h> /* NR_syscalls */

#include "core.h"

extern unsigned long *sct; /* Syscall Table */
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */



void analyze_syscalls(void){
	printk("testing syscall!");
}
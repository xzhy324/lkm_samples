#include <asm/asm-offsets.h> /* NR_syscalls */

#include "core.h"
#include "util.h"

#if MUTEX_IS_NOT_PREDEFINED
static DEFINE_MUTEX(module_mutex);
#endif
extern unsigned long *sct; /* Syscall Table */
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */



void analyze_syscalls(void){
	int i;
    unsigned long addr;
    struct module *mod;
    const char *mod_name;
    printk("[sys_inspector.ko] Analyzing syscall...");

    for (i=0; i< NR_syscalls; i++){
        addr = sct[i];
        if (!ckt(addr)) {
			printk(KERN_ALERT"[sys_inspector.ko] syscall[%d] addr:%lx is NOT in kernel .text segment!", i, addr);
            mutex_lock(&module_mutex);
            mod = get_module_from_addr(addr);
            if (mod){
                printk(KERN_ALERT"[sys_inspector.ko] Module [%s] is hooking!\n",mod->name);
            } else {
                mod_name = find_hidden_module(addr);
                if (mod_name)
                    printk(KERN_ALERT"[sys_inspector.ko] Hidden Module [%s] is hooking!\n",mod_name);
            }
            mutex_unlock(&module_mutex);
		}
        
    }

}
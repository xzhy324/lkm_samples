//hello.c
#include <linux/init.h>  
#include <linux/module.h> 
//kallsyms_lookup_name
#include <linux/ftrace.h>
#include <linux/kprobes.h>
// NR_syscalls, __NR_uname
#include <asm/asm-offsets.h>
MODULE_LICENSE("GPL"); 

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

unsigned long *syscall_table = NULL;
int (*ckt)(unsigned long addr) = NULL;

void check_syscalltable(void) {
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    syscall_table = kallsyms_lookup_name("sys_call_table");
    ckt = kallsyms_lookup_name("core_kernel_text"); 

    printk(KERN_ALERT "[detect.ko] syscall_table is at %p", syscall_table);
    printk(KERN_ALERT "[detect.ko] core_kernel_text(addr) is at %p", ckt);
	printk(KERN_ALERT "[detect.ko] NR_syscalls: %d", NR_syscalls);
	//printk(KERN_ALERT "[detect.ko] __NR_uname: %d", __NR_uname);

    int i;
    unsigned long addr;
    for (i=0; i< NR_syscalls; i++){
        addr = syscall_table[i];
        if (!ckt(addr)) {
			printk(KERN_ALERT"[detect.ko] syscall[%d] addr:%p lays in ring3!", i, addr);
		}
    }
}


static int h_init(void)  
{  
  printk(KERN_ALERT "[detect.ko] initing ...\n");
  check_syscalltable();

  return 0;  
}

static void h_exit(void)  
{  
  printk(KERN_ALERT "[detect.ko] removing ..\n");  
}  
    
module_init(h_init);  
module_exit(h_exit);
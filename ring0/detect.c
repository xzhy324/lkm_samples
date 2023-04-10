// #include <linux/init.h>
#include <linux/module.h>
/*kallsyms_lookup_name*/
#include <linux/kprobes.h>
/*NR_syscalls, __NR_uname*/
#include <asm/asm-offsets.h>
MODULE_LICENSE("GPL");

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"};

unsigned long *syscall_table = NULL;
int (*ckt)(unsigned long addr) = NULL;
unsigned long startup_64 = NULL;

void check_syscalltable(void)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
    printk("[detect.ko] kallsyms_lookup is at %lx", kallsyms_lookup_name);
    syscall_table = kallsyms_lookup_name("sys_call_table");
    ckt = kallsyms_lookup_name("core_kernel_text");
    startup_64 = kallsyms_lookup_name("startup_64");

    // printk(KERN_ALERT "[detect.ko] syscall_table is at %lx", syscall_table);
    // printk(KERN_ALERT "[detect.ko] core_kernel_text(addr) is at %lx", ckt);
    // printk(KERN_ALERT "[detect.ko] NR_syscalls: %d", NR_syscalls);

    printk(KERN_ALERT "[detect.ko] (startup_64,va,pa) is startup_64 %lx %lx", startup_64, __pa(startup_64));
    printk(KERN_ALERT "[detect.ko] startup_64 is at %lx", startup_64);
    printk(KERN_ALERT "[detect.ko] sprint_symbol is at %lx", &sprint_symbol);
    printk(KERN_ALERT "[detect.ko] sprint_symbol - startup_64 = %lx", &sprint_symbol - startup_64);
    printk(KERN_ALERT "[detect.ko] sprint_symbol(pa) is at %lx", __pa(&sprint_symbol));
    printk(KERN_ALERT "[detect.ko] startup_64(pa) is at %lx", __pa(startup_64));
    printk(KERN_ALERT "[detect.ko] sprint_symbol(pa) - startup_64(pa) = %lx", __pa(&sprint_symbol) - __pa(startup_64));
    printk(KERN_ALERT "[detect.ko] sys_call_table(pa) is at %lx", __pa(syscall_table));
    printk(KERN_ALERT "[detect.ko] kallsyms_lookup_name(pa) - startup_64(pa) = %lx", __pa(kallsyms_lookup_name) - __pa(startup_64));
    printk(KERN_ALERT "[detect.ko] syscall_table(pa) - startup_64(pa) = %lx", __pa(syscall_table) - __pa(startup_64));

    int i;
    unsigned long addr;
    for (i = 0; i < NR_syscalls; i++)
    {
        addr = syscall_table[i];
        if (!ckt(addr))
        {
            printk(KERN_ALERT "[detect.ko] syscall[%d] addr:%lx lays in ring3!", i, addr);
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
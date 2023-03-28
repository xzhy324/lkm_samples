// #include <linux/init.h>
#include <linux/module.h>
/*kallsyms_lookup_name*/
#include <linux/kprobes.h>
MODULE_LICENSE("GPL");

#define __START_KERNEL_map 0xffffffff80000000

char *kobjs[] = {
    "sys_call_table",
    "core_kernel_text",
    "sprint_symbol",
    "idt_table",
    "kallsyms_lookup_name"
};

unsigned long kernel_base = NULL; // startup_64

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"};

void check_syscalltable(void)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
    kernel_base = kallsyms_lookup_name("startup_64");
    printk("[test.ko] phys_base is %lx", phys_base);
    printk("[test.ko] v2p:startup_64 is %lx", kernel_base - __START_KERNEL_map + phys_base);
    printk("[test.ko] startup_64(va) is at %lx", kernel_base);
    printk("[test.ko] startup_64(pa) is at %lx", __pa(kernel_base));
    int i;
    int kobjs_size = sizeof(kobjs) / sizeof(kobjs[0]);
    for (i = 0; i < kobjs_size; i++)
    {
        unsigned long addr = kallsyms_lookup_name(kobjs[i]);
        printk("[test.ko] %s(va) is at %lx", kobjs[i], addr);
        printk("[test.ko] %s(va) - startup_64(va) = %lx", kobjs[i], addr - kernel_base);
        printk("[test.ko] %s(pa) - startup_64(pa) = %lx", kobjs[i], __pa(addr) - __pa(kernel_base));
    }
}

static int h_init(void)
{
    printk("[test.ko] initing ...\n");
    check_syscalltable();

    return 0;
}

static void h_exit(void)
{
    printk("[test.ko] removing ..\n");
}

module_init(h_init);
module_exit(h_exit);
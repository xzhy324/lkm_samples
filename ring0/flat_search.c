#include <linux/module.h>
#include <linux/kallsyms.h> //sprintf_symbol
#include <linux/slab.h>     //kmalloc, kfree

unsigned long *syscall_table = NULL;

unsigned long lookup_name(const char *name)
{
    // 使用kmalloc申请一块用于保存函数名的空间
    char *name_buffer = kmalloc(strlen(name) + 1, GFP_KERNEL);
    unsigned long kaddr = (unsigned long)&sprint_symbol;
    int i;
    
    kaddr &= 0xffffffffff000000;
    for (i = 0x0; i < 0x100000; i++)
    {
        sprint_symbol(name_buffer, kaddr);

        if (strncmp(name_buffer, name, strlen(name)) == 0)
        {
            /* Match! Clean up and exit */
            kfree(name_buffer);
            return kaddr;
        }

        /* Kernel function addresses are all aligned, so we skip 0x10 bytes */
        kaddr += 0x10;
    }
    kfree(name_buffer);
    return kaddr;
}

void check_syscalltable(void)
{
    syscall_table = lookup_name("startup_64");

    printk(KERN_ALERT "[flat_search.ko] syscall_table is at %lx", syscall_table);
}

static int h_init(void)
{
    printk(KERN_ALERT "[flat_search.ko] initing ...\n");
    check_syscalltable();

    return 0;
}

static void h_exit(void)
{
    printk(KERN_ALERT "[flat_search.ko] removing ..\n");
}

module_init(h_init);
module_exit(h_exit);
MODULE_LICENSE("GPL");
#include <linux/module.h>


//runtime base
#define phys_base 0xee600000


//static offset
#define START_KERNEL_map 0xffffffff80000000
#define startup_64 ffffffff81000000
#define sys_call_table ffffffff82200300
#define SYSCALL_TABLE_OFFSET (startup_64 - sys_call_table)
#define __pa(x) ((unsigned long)(x) - START_KERNEL_map + phys_base)
#define __va(x) ((void *)((unsigned long)(x) - phys_base + START_KERNEL_map))
 
static int lkm_init(void)
{
    
    return 0;
}

static void lkm_exit(void)
{
    printk("Arciryas:moduleremoved\n");
}
 
module_init(lkm_init);
module_exit(lkm_exit);
MODULE_LICENSE("GPL");
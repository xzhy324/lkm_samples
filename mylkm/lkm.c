/*lkm.c*/
 
#include <linux/module.h>
#include <linux/kernel.h>
//#include <linux/init.h>
 
static int lkm_init(void)
{
    printk("Arciryas:moduleloaded\n");
    return 0;
}

static void lkm_exit(void)
{
    printk("Arciryas:moduleremoved\n");
}
 
module_init(lkm_init);
module_exit(lkm_exit);
MODULE_LICENSE("GPL");
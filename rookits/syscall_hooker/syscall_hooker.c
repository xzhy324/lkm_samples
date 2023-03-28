#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>   //kallsyms_lookup_name
#include <asm/asm-offsets.h> // NR_syscalls, __NR_uname

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"};

unsigned long *syscall_table = NULL;

typedef asmlinkage long (*orig_mkdir_t)(const struct pt_regs *);
orig_mkdir_t orig_mkdir;

inline void mywrite_cr0(unsigned long cr0)
{
    asm volatile("mov %0,%%cr0" : "+r"(cr0));
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
asmlinkage int hook_function(const struct pt_regs *regs)
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
    orig_mkdir(regs);
    return 0;
}

void hook_syscalltable(void)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
    syscall_table = kallsyms_lookup_name("sys_call_table");

    printk(KERN_ALERT "[syscall_hooker.ko] syscall_table is at %lx", syscall_table);
    printk(KERN_ALERT "[syscall_hooker.ko] NR_syscalls: %d", NR_syscalls);
    printk(KERN_ALERT "[syscall_hooker.ko] __NR_mkdir: %d", __NR_mkdir);

    orig_mkdir = (orig_mkdir_t)syscall_table[__NR_mkdir]; // 保存nr_uname调用的真实地址
    disable_write_protection();                           // 关闭内核写保护
    syscall_table[__NR_mkdir] = (unsigned long)hook_function;
    enable_write_protection();
    printk(KERN_ALERT "[syscall_hooker.ko] mkdir has been hooked!!!");
}

static int h_init(void)
{
    printk(KERN_ALERT "[syscall_hooker.ko] initing ...\n");
    hook_syscalltable();
    return 0;
}

static void h_exit(void)
{
    disable_write_protection();
    syscall_table[__NR_mkdir] = (unsigned long)orig_mkdir;
    enable_write_protection();
    printk(KERN_ALERT "[syscall_hooker.ko] removing ..\n");
}

module_init(h_init);
module_exit(h_exit);
MODULE_LICENSE("GPL");
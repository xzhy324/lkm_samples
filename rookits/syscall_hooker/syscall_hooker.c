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
unsigned int old_uname;
unsigned long __force_order;

inline void mywrite_cr0(unsigned long cr0) {
  asm volatile("mov %0,%%cr0" : "+r"(cr0), "+m"(__force_order));
}
void enable_write_protection(void) {
  unsigned long cr0 = read_cr0();
  set_bit(16, &cr0);
  mywrite_cr0(cr0);
}
void disable_write_protection(void) {
  unsigned long cr0 = read_cr0();
  clear_bit(16, &cr0);
  mywrite_cr0(cr0);
}
void hook_function(void)
{
  printk(KERN_ALERT "syscall 63 uname has been hooked!!\n");
  return;
}

void hook_syscalltable(void) {
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    syscall_table = kallsyms_lookup_name("sys_call_table");
    ckt = kallsyms_lookup_name("core_kernel_text"); 

    printk(KERN_ALERT "[syscall_hooker.ko] syscall_table is at %p", syscall_table);
    printk(KERN_ALERT "[syscall_hooker.ko] core_kernel_text(addr) is at %p", ckt);
	printk(KERN_ALERT "[syscall_hooker.ko] NR_syscalls: %d", NR_syscalls);
	printk(KERN_ALERT "[syscall_hooker.ko] __NR_uname: %d", __NR_uname);

    disable_write_protection();//关闭内核写保护
    old_uname = (unsigned int)syscall_table[__NR_uname];//保存nr_uname调用的真实地址
    syscall_table[__NR_uname] = hook_function;
    enable_write_protection();
    printk(KERN_ALERT "[syscall_hooker.ko] uname has been hooked!!!");


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
	syscall_table[__NR_uname] = old_uname;
	enable_write_protection();
  	printk(KERN_ALERT "[syscall_hooker.ko] removing ..\n");  
}  
    
module_init(h_init);  
module_exit(h_exit);
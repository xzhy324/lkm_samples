#include <linux/init.h>
#include <linux/module.h>
#include <linux/kprobes.h>  /*kallsyms_lookup_name*/
#include <asm/asm-offsets.h>/*NR_syscalls, __NR_uname*/
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/export.h>
#include <asm-generic/vmlinux.lds.h> /*LOAD_OFFSET*/

static unsigned long cr0, cr3;

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"};

unsigned long *syscall_table = NULL;
int (*ckt)(unsigned long addr) = NULL;

void check_syscalltable(void)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
    printk("[syscall_pa.ko] kallsyms_lookup is at %lx", kallsyms_lookup_name);
    syscall_table = kallsyms_lookup_name("sys_call_table");
    ckt = kallsyms_lookup_name("core_kernel_text");

    printk(KERN_ALERT "[syscall_pa.ko] syscall_table is at %lx", syscall_table);
    printk(KERN_ALERT "[syscall_pa.ko] core_kernel_text(addr) is at %lx", ckt);
    printk(KERN_ALERT "[syscall_pa.ko] NR_syscalls: %d", NR_syscalls);
}

/*打印页中一些重要参数*/
static void get_pgtable_macro(void)
{
    cr0 = read_cr0();    /*读取cr0的值*/
    cr3 = read_cr3_pa(); /*读取cr3的值*/

    printk("[syscall_pa.ko] cr0 = 0x%lx, cr3 = 0x%lx\n", cr0, cr3);
    /* *_SHIFT宏表示在线性地址中相应字段所能映射地址大小的对数
    比如PAGE_SHIFT，我们知道页面就是page_size=4K，PAGE_SHIFT=log2(page_size)=12
    */
    printk("[syscall_pa.ko] PGDIR_SHIFT = %d\n", PGDIR_SHIFT);
    printk("[syscall_pa.ko] P4D_SHIFT   = %d\n", P4D_SHIFT);
    printk("[syscall_pa.ko] PUD_SHIFT   = %d\n", PUD_SHIFT);
    printk("[syscall_pa.ko] PMD_SHIFT   = %d\n", PMD_SHIFT);
    printk("[syscall_pa.ko] PAGE_SHIFT  = %d\n", PAGE_SHIFT);
    /*PTRS_PER_*宏表示该页目录表中项的个数*/
    printk("[syscall_pa.ko] PTRS_PER_PGD = %d\n", PTRS_PER_PGD);
    printk("[syscall_pa.ko] PTRS_PER_P4D = %d\n", PTRS_PER_P4D);
    printk("[syscall_pa.ko] PTRS_PER_PUD = %d\n", PTRS_PER_PUD);
    printk("[syscall_pa.ko] PTRS_PER_PMD = %d\n", PTRS_PER_PMD);
    printk("[syscall_pa.ko] PTRS_PER_PTE = %d\n", PTRS_PER_PTE);
    /*PAGE_MASK页内偏移*/
    printk("[syscall_pa.ko] PAGE_MASK    = 0x%lx\n", PAGE_MASK);
}

static unsigned long vaddr2paddr(unsigned long vaddr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long paddr = 0;
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;
    /*current->mm：当前进程的mm_struct结构
        vaddr虚拟地址
        return 该虚拟地址对应的pgd地址
        pgd_offset展开后：
        pgd = current->mm->pgd + (vaddr >> PGDDIR_SHIFT) & (PTRS_PER_PGD -1)
    */
    pgd = pgd_offset(current->mm, vaddr);
    /*pgd_val(*pgd)打印pgd地址的值
        pgd_index(vaddr)打印vaddr在页全局目录的索引
    */
    printk("[syscall_pa.ko] pgd_val = 0x%lx, pgd_index = %lu\n", pgd_val(*pgd), pgd_index(vaddr));
    if (pgd_none(*pgd))
    {
        printk("[syscall_pa.ko] not mapped in pgd\n");
        return -1;
    }
    /*p4d由于没有启用，所以目录表项为1，即p4d=pgd*/
    p4d = p4d_offset(pgd, vaddr);
    printk("[syscall_pa.ko] p4d_val = 0x%lx, p4d_index = %lu\n", p4d_val(*p4d), p4d_index(vaddr));
    if (p4d_none(*p4d))
    {
        printk("[syscall_pa.ko] not mapped in p4d\n");
        return -1;
    }

    /*
    pud = *pgd & PAGE_MASK + (vaddr >> PUD_SHIFT) & (PTRS_PER_PUD -1) * sizeof(pud_t);
    */
    pud = pud_offset(p4d, vaddr);
    printk("[syscall_pa.ko] pud_val = 0x%lx, pud_index = %lu\n", pud_val(*pud), pud_index(vaddr));
    if (pud_none(*pud))
    {
        printk("[syscall_pa.ko] not mapped in pud\n");
        return -1;
    }
    /*
    pmd = *pud & PAGE_MASK + (vaddr >> PMD_SHIFT) & 		(PTRS_PER_PMD -1) * sizeof(pmd_t);
    */
    pmd = pmd_offset(pud, vaddr);
    printk("[syscall_pa.ko] pmd_val = 0x%lx, pmd_index = %lu\n", pmd_val(*pmd), pmd_index(vaddr));
    if (pmd_none(*pmd))
    {
        printk("[syscall_pa.ko] not mapped in pmd\n");
        return -1;
    }
    /*
    pmd = *pmd & PAGE_MASK + (vaddr >> PAGE_SHIFT) & 		(PTRS_PER_PTE -1) * sizeof(pte_t);
    */
    pte = pte_offset_kernel(pmd, vaddr);
    printk("[syscall_pa.ko] pte_val = 0x%lx, pte_index = %lu\n", pte_val(*pte), pte_index(vaddr));
    if (pte_none(*pte))
    {
        printk("[syscall_pa.ko] not mapped in pte\n");
        return -1;
    }
    /*pte地址的值加上页内偏移，就是物理地址*/
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    printk("[syscall_pa.ko] page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
    printk("[syscall_pa.ko] vaddr = %lx, paddr = %lx\n", vaddr, paddr);
    return paddr;
}

static int h_init(void)
{
    printk(KERN_ALERT "[syscall_pa.ko] initing ...\n");
    // 初始化syscall)table
    check_syscalltable();
    printk("[syscall_pa.ko] PAGE_OFFSET:%lx", PAGE_OFFSET);
    printk("[syscall_pa.ko] __PAGE_OFFSET:%lx", __PAGE_OFFSET);
    printk("[syscall_pa.ko] __START_KERNEL_map:%lx", __START_KERNEL_map);
    printk("[syscall_pa.ko] LOAD_OFFSET:%lx", LOAD_OFFSET);
    printk("[syscall_pa.ko] phys_base:%lx", phys_base);
    printk("[syscall_pa.ko] pa of phys_base:%lx", __pa(&phys_base));

    printk("[syscall_pa.ko] syscall_table vaddr in %%lx: %lx", syscall_table);
    // printk("[syscall_pa.ko] syscall_table vaddr in %%p : %p", syscall_table);
    printk("[syscall_pa.ko] _pa(sys_call_table): %lx", __pa(syscall_table));
    printk("[syscall_pa.ko] __phys_addr_symbol(sys_call_table): %lx", __phys_addr_symbol(syscall_table));
    vaddr2paddr(syscall_table);
    // 验证system.map中给出的虚拟地址
    // printk("[syscall_pa.ko] syscall_table from system.map:ffffffff82200300");
    // vaddr2paddr(0xffffffff82200300);
    return 0;
}

static void h_exit(void)
{
    printk(KERN_ALERT "[syscall_pa.ko] removing ..\n");
}

module_init(h_init);
module_exit(h_exit);
MODULE_LICENSE("GPL");
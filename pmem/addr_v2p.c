/*
 * 主要作用是在内核中先申请一个页面，使用内核提供的函数，按照寻页的步骤一步步的找到物理地址。
 * 这些步骤就相当于我们手动的模拟了mmu的寻页过程。
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/export.h>

static unsigned long cr0, cr3;

static unsigned long vaddr = 0;

/*打印页中一些重要参数*/
static void get_pgtable_macro(void)
{
	cr0 = read_cr0();	 /*读取cr0的值*/
	cr3 = read_cr3_pa(); /*读取cr3的值*/

	printk("cr0 = 0x%lx, cr3 = 0x%lx\n", cr0, cr3);
	/* *_SHIFT宏表示在线性地址中相应字段所能映射地址大小的对数
	比如PAGE_SHIFT，我们知道页面就是page_size=4K，PAGE_SHIFT=log2(page_size)=12
	*/
	printk("PGDIR_SHIFT = %d\n", PGDIR_SHIFT);
	printk("P4D_SHIFT   = %d\n", P4D_SHIFT);
	printk("PUD_SHIFT   = %d\n", PUD_SHIFT);
	printk("PMD_SHIFT   = %d\n", PMD_SHIFT);
	printk("PAGE_SHIFT  = %d\n", PAGE_SHIFT);
	/*PTRS_PER_*宏表示该页目录表中项的个数*/
	printk("PTRS_PER_PGD = %d\n", PTRS_PER_PGD);
	printk("PTRS_PER_P4D = %d\n", PTRS_PER_P4D);
	printk("PTRS_PER_PUD = %d\n", PTRS_PER_PUD);
	printk("PTRS_PER_PMD = %d\n", PTRS_PER_PMD);
	printk("PTRS_PER_PTE = %d\n", PTRS_PER_PTE);
	/*PAGE_MASK页内偏移*/
	printk("PAGE_MASK    = 0x%lx\n", PAGE_MASK);
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
	printk("pgd_val = 0x%lx, pgd_index = %lu\n", pgd_val(*pgd), pgd_index(vaddr));
	if (pgd_none(*pgd))
	{
		printk("not mapped in pgd\n");
		return -1;
	}
	/*p4d由于没有启用，所以目录表项为1，即p4d=pgd*/
	p4d = p4d_offset(pgd, vaddr);
	printk("p4d_val = 0x%lx, p4d_index = %lu\n", p4d_val(*p4d), p4d_index(vaddr));
	if (p4d_none(*p4d))
	{
		printk("not mapped in p4d\n");
		return -1;
	}

	/*
	pud = *pgd & PAGE_MASK + (vaddr >> PUD_SHIFT) & (PTRS_PER_PUD -1) * sizeof(pud_t);
	*/
	pud = pud_offset(p4d, vaddr);
	printk("pud_val = 0x%lx, pud_index = %lu\n", pud_val(*pud), pud_index(vaddr));
	if (pud_none(*pud))
	{
		printk("not mapped in pud\n");
		return -1;
	}
	/*
	pmd = *pud & PAGE_MASK + (vaddr >> PMD_SHIFT) & 		(PTRS_PER_PMD -1) * sizeof(pmd_t);
	*/
	pmd = pmd_offset(pud, vaddr);
	printk("pmd_val = 0x%lx, pmd_index = %lu\n", pmd_val(*pmd), pmd_index(vaddr));
	if (pmd_none(*pmd))
	{
		printk("not mapped in pmd\n");
		return -1;
	}
	/*
	pmd = *pmd & PAGE_MASK + (vaddr >> PAGE_SHIFT) & 		(PTRS_PER_PTE -1) * sizeof(pte_t);
	*/
	pte = pte_offset_kernel(pmd, vaddr);
	printk("pte_val = 0x%lx, pte_index = %lu\n", pte_val(*pte), pte_index(vaddr));
	if (pte_none(*pte))
	{
		printk("not mapped in pte\n");
		return -1;
	}
	/*pte地址的值加上页内偏移，就是物理地址*/
	page_addr = pte_val(*pte) & PAGE_MASK;
	page_offset = vaddr & ~PAGE_MASK;
	paddr = page_addr | page_offset;
	printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
	printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);
	printk("using __pa(), paddr is %lx\n", __pa(vaddr));
	return paddr;
}

static int __init v2p_init(void)
{
	unsigned long vaddr = 0;
	printk("vaddr to paddr module is running..\n");
	get_pgtable_macro();
	printk("\n");
	/*申请一个缺页的页面*/
	vaddr = __get_free_page(GFP_KERNEL);
	if (vaddr == 0)
	{
		printk("__get_free_page failed..\n");
		return -1;
	}
	/*往虚拟地址写hello world用于验证逻辑*/
	sprintf((char *)vaddr, "hello world from kernel");
	printk("get_page_vaddr=0x%lx\n", vaddr);
	vaddr2paddr(vaddr);
	return 0;
}

static void __exit v2p_exit(void)
{
	printk("vaddr to paddr module is leaving..\n");
	free_page(vaddr);
}

module_init(v2p_init);
module_exit(v2p_exit);
MODULE_LICENSE("GPL");
#include "core.h"
#include "util.h"
#include "proc.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
#include <linux/sched/signal.h>
#else
#include <linux/sched.h>
#endif

#if MUTEX_IS_NOT_PREDEFINED
static DEFINE_MUTEX(module_mutex);
#endif
extern int (*ckt)(unsigned long addr); /* Core Kernel Text */

/* https://elixir.bootlin.com/linux/latest/source/fs/nfsd/vfs.c */
static int filldir_fn(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int type){
	struct readdir_data *buf;
	struct linux_dirent *d;
	unsigned int reclen;

	buf = container_of(ctx, struct readdir_data, ctx);
	d = (void *)(buf->dirent + buf->used);

	reclen = ALIGN(sizeof(struct linux_dirent) + namlen, sizeof(u64));
	if (buf->used + reclen > PAGE_SIZE) {
		buf->full = 1;
		return -EINVAL;
	}

	d->d_ino = ino;
	d->d_off = offset;
	d->d_namlen = namlen;
	d->d_type = type;
	memcpy(d->d_name, name, namlen);
	buf->used += reclen;

	return 0;
}

static void analyze_inodes(void){
	int size, error;
	char *buffer;
	struct file *fp;
	struct linux_dirent *d;
	struct readdir_data buf = {
		.ctx.actor = filldir_fn,
		.dirent = (void *)__get_free_page(GFP_KERNEL),
	};

	printk("[sys_inspector.ko] Analyzing /proc Inodes\n");

	fp = filp_open("/proc", O_RDONLY, S_IRUSR);
	if (IS_ERR(fp)){
		pr_err("[sys_inspector.ko] Failed to open /proc.");
		return;
	}

	while (1){
		unsigned int reclen;
		buf.used = 0;
		buf.full = 0;

		error = iterate_dir(fp, &buf.ctx);
		if ((!buf.full && (error < 0)) || !buf.used)
			break;

		size = buf.used;
		d = (struct linux_dirent *)buf.dirent;

		while (size > 0){
			if (d->d_ino == 0){
				buffer = kzalloc(d->d_namlen+1, GFP_KERNEL);
				memcpy(buffer, d->d_name, d->d_namlen);
				pr_alert("[sys_inspector.ko] Hidden Process [/proc/%s].\n", buffer);
				kfree(buffer);
			}

			reclen = ALIGN(sizeof(*d) + d->d_namlen, sizeof(u64));
			d = (struct linux_dirent *)((char *)d + reclen);
			size -= reclen;
		}

		if (size > 0)
			break;
	}

	free_page((unsigned long)(buf.dirent));
}

void analyze_fops(void) {
    unsigned long addr;
	const char *mod_name;
	struct file *fp;
	struct module *mod;

	printk("[sys_inspector.ko] Analyzing /proc File Operations...\n");

	fp = filp_open("/proc", O_RDONLY, S_IRUSR);
	if (IS_ERR(fp)){
		printk("[sys_inspector] Failed to open /proc.");
		return;
	}

	if (IS_ERR(fp->f_op)){
		printk("[sys_inspector] /proc has no fops.");
		return;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	addr = (unsigned long)fp->f_op->iterate;
#else
	addr = (unsigned long)fp->f_op->readdir;
#endif

	if (!ckt(addr)){
		mutex_lock(&module_mutex);
		mod = get_module_from_addr(addr);
		if (mod){
			printk("[sys_inspector] Module [%s] hijacked /proc fops.\n", mod->name);
		} else {
			mod_name = find_hidden_module(addr);
			if (mod_name){
				printk("[sys_inspector] Module [%s] hijacked /proc fops.\n", mod_name);
			}
		}
		mutex_unlock(&module_mutex);
	}
}

void analyze_procs(void) {
    analyze_fops();
	analyze_inodes();
}
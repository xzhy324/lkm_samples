#ifndef NETWORK_PORT_H
#define NETWORK_PORT_H

#include <linux/fs.h>
#include <linux/proc_fs.h>


/*
	struct proc_dir_entry was made opaque in 3.10 with the following commit.
	We will make it available to us by redefining it. This isn't the best practice, but it works.
	https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/include/linux/proc_fs.h?id=59d8053f1e16904d54ed7469d4b36801ea6b8f2c
*/

struct proc_dir_entry {
	/*
	 * number of callers into module in progress;
	 * negative -> it's going away RSN
	 */
	atomic_t in_use;
	refcount_t refcnt;
	struct list_head pde_openers;	/* who did ->open, but not ->release */
	/* protects ->pde_openers and all struct pde_opener instances */
	spinlock_t pde_unload_lock;
	struct completion *pde_unload_completion;
	const struct inode_operations *proc_iops;
	union {
		const struct proc_ops *proc_ops;
		const struct file_operations *proc_dir_ops;
	};
	const struct dentry_operations *proc_dops;
	union {
		const struct seq_operations *seq_ops;
		int (*single_show)(struct seq_file *, void *);
	};
	proc_write_t write;
	void *data;
	unsigned int state_size;
	unsigned int low_ino;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	struct proc_dir_entry *parent;
	struct rb_root subdir;
	struct rb_node subdir_node;
	char *name;
	umode_t mode;
	u8 flags;
	u8 namelen;
	char inline_name[];
};

struct net_entry {
	const char *name;
	struct proc_dir_entry *entry;	
};

void analyze_networks(void);

#endif /* NETWORK_PORT_H */
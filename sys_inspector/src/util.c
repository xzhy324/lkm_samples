#include "core.h"
#include "util.h"

#include <linux/kprobes.h>

static struct kprobe kp;

unsigned long lookup_name(const char *name){
	kp.symbol_name = name;

	if (register_kprobe(&kp) < 0)
		return 0;

	unregister_kprobe(&kp);

	return (unsigned long)kp.addr;
}
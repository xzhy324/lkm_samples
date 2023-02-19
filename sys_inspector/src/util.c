#include <linux/kprobes.h>
#include <linux/kallsyms.h>

#include "core.h"
#include "util.h"

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

unsigned long lookup_name(const char *name){
	typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
	return (unsigned long)kallsyms_lookup_name(name);
}

struct module *get_module_from_addr(unsigned long addr){
    typedef struct module* (*module_address_t)(unsigned long addr);
    module_address_t module_address = (module_address_t)lookup_name("__module_address");
    return module_address(addr);
}


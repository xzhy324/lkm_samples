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

#define BETWEEN_PTR(x, y, z) ( \
	((uintptr_t)x >= (uintptr_t)y) && \
	((uintptr_t)x < ((uintptr_t)y+(uintptr_t)z)) \
)

const char *find_hidden_module(unsigned long addr){
    //printk("in find_hidden_module:addr:%lx",addr);
	const char *mod_name = NULL;
	struct kset *mod_kset;
	struct kobject *cur, *tmp;
	struct module_kobject *kobj;

	mod_kset = (void *)lookup_name("module_kset");
	if (!mod_kset)
		return NULL;

	list_for_each_entry_safe(cur, tmp, &mod_kset->list, entry){
		if (!kobject_name(tmp))
			break;

		kobj = container_of(tmp, struct module_kobject, kobj);
		if (!kobj || !kobj->mod)
			continue;

		if (BETWEEN_PTR(addr, kobj->mod->core_layout.base, kobj->mod->core_layout.size)){
			mod_name = kobj->mod->name;
		}
        //printk("%s %lx %lx",kobj->mod->name,kobj->mod->core_layout.base,kobj->mod->core_layout.size);

	}

	return mod_name;
}

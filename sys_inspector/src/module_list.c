#include "core.h"
#include "util.h"
#include "module_list.h"

static DEFINE_MUTEX(module_mutex);

struct module *find_module1(unsigned long addr){
    typedef struct module* (*find_module_t)(unsigned long addr);
    find_module_t find_mod = (find_module_t)lookup_name("find_module");
    //printk("[debug] find_module is at %lx",(unsigned long)find_mod);
    return find_mod(addr);
}

void analyze_modules(void){
    struct kset *mod_kset;
	struct kobject *cur, *tmp;
	struct module_kobject *kobj;

    printk("[sys_inspector.ko] analyzing modules...");
	mod_kset = (void *)lookup_name("module_kset");
	if (!mod_kset)
		return;

	list_for_each_entry_safe(cur, tmp, &mod_kset->list, entry){
		if (!kobject_name(tmp))
			break;

		kobj = container_of(tmp, struct module_kobject, kobj);

		if (kobj && kobj->mod && kobj->mod->name){
			mutex_lock(&module_mutex);
			if(!find_module1(kobj->mod->name)){
				printk(KERN_ALERT"Module [%s] hidden.\n", kobj->mod->name);
			}else {
				//printk("[sys_inspector.ko] Module [%s] passed check", kobj->mod->name);
			}
            mutex_unlock(&module_mutex);
		}
	}
}
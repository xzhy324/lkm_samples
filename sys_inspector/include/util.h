#ifndef UTIL_H
#define UTIL_H

extern unsigned long lookup_name(const char *);
struct module *get_module_from_addr(unsigned long);
const char *find_hidden_module(unsigned long);



#endif /* UTIL_H */
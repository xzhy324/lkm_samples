KDIR:=/lib/modules/$(shell uname -r)/build
# PWD:=$(shell pwd)
 
# ===================================
SYSINSPECTOR := sys_inspector
SRCDIR := src
ORIGIN := $(PWD)

obj-m := $(SYSINSPECTOR).o
$(SYSINSPECTOR)-y += $(SRCDIR)/core.o
$(SYSINSPECTOR)-y += $(SRCDIR)/util.o
$(SYSINSPECTOR)-y += $(SRCDIR)/syscall_table.o
$(SYSINSPECTOR)-y += $(SRCDIR)/idt_table.o
$(SYSINSPECTOR)-y += $(SRCDIR)/module_list.o

HEADERS := $(PWD)/include
ccflags-y += -I$(HEADERS)

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean: 
	make -C $(KDIR) M=$(PWD) clean


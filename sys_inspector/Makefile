KDIR:=/usr/src/linux-headers-$(shell uname -r)
# PWD:=$(shell pwd)
 
# ===================================
SYSINSPECTOR := sys_inspector
SRCDIR := src
ORIGIN := $(PWD)

obj-m := $(SYSINSPECTOR).o
$(SYSINSPECTOR)-y += $(SRCDIR)/core.o
$(SYSINSPECTOR)-y += $(SRCDIR)/util.o
$(SYSINSPECTOR)-y += $(SRCDIR)/syscall_table.o

HEADERS := $(PWD)/include
ccflags-y += -I$(HEADERS)

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	cp sys_inspector.ko $(DESTDIR)/

clean: clean-module

clean-module:
	make -C $(KDIR) M=$(PWD) clean


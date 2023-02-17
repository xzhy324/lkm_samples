CONFIG_MODULE_SIG=n

obj-m:=lkm.o
 
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
 
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o  .*.cmd *.mod.c .tmp_versions Module.symvers modules.order 

test:
	$(MAKE) -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) modules
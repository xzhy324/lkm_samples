
obj-m :=syscall_hooker.o  
KERNEL :=/lib/modules/$(shell uname -r)/build 
PWD :=$(shell pwd)  
modules:  
	$(MAKE) -C $(KERNEL) M=$(PWD) modules  
.PHONEY:clean  
clean:  
	rm -rf *.o  .*.cmd *.mod.c .tmp_versions Module.symvers modules.order
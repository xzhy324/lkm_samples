
obj-m :=syscall_hooker.o  
KERNEL :=/lib/modules/$(shell uname -r)/build 
PWD :=$(shell pwd)  
modules:  
	$(MAKE) -C $(KERNEL) M=$(PWD) modules  

clean:  
	make -C $(KERNEL) M=$(PWD) clean
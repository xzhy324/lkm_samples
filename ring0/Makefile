obj-m :=detect.o flat_search.o export_offset.o test.o
KERNEL :=/lib/modules/$(shell uname -r)/build 
PWD :=$(shell pwd)  
modules:  
	$(MAKE) -C $(KERNEL) M=$(PWD) modules  
.PHONEY:clean  
clean:  
	make -C $(KERNEL) M=$(PWD) clean
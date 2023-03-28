obj-m:=addr_v2p.o dram.o      
CURRENT_PATH := $(shell pwd)       
LINUX_KERNEL := $(shell uname -r)   
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL) 

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
	gcc fileview.cpp -o fileview	

install: all
	sudo insmod dram.ko
	sudo insmod addr_v2p.ko
	sudo mknod /dev/dram c 85 0   #major_dev_id(85) is specified in dram.c

uninstall:clean
	sudo rmmod dram.ko
	sudo rmmod addr_v2p.ko
	sudo rm /dev/dram

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean 
	rm ./fileview
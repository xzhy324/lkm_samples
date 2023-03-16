obj-m:=addr_v2p.o dram.o             #产生1_hello模块的目标文件
CURRENT_PATH := $(shell pwd)            #模块所在的当前路径
LINUX_KERNEL := $(shell uname -r)       #Linux内核源码代码的当前版本
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)     #Linux内核源代码的绝对路径

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules  #编译模块
	gcc fileview.cpp -o fileview	

install:
	sudo insmod dram.ko
	sudo insmod addr_v2p.ko
	sudo mknod /dev/dram c 85 0

uninstall:
	sudo rmmod dram.ko
	sudo rmmod addr_v2p.ko
	sudo rm /dev/dram

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean    #清理模块
	rm ./fileview
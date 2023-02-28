obj-m:=addr_v2p.o                #产生1_hello模块的目标文件
CURRENT_PATH := $(shell pwd)            #模块所在的当前路径
LINUX_KERNEL := $(shell uname -r)       #Linux内核源码代码的当前版本
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)     #Linux内核源代码的绝对路径

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules  #编译模块

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean    #清理模块
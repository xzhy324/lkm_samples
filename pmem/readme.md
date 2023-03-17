# Phisical Memory Viewer
## Code Introduction
### addr_v2p.c
* 主要作用是在内核中先申请一个页面，使用内核提供的函数，按照寻页的步骤一步步的找到物理地址。
* 这些步骤就相当于我们手动的模拟了mmu的寻页过程。
### dram.c
* 旧金山大学高级编程课上找的小工具，主要是通过mmap将物理内存的数据映射到一个设备文件中。
* 通过访问该设备就可以实现访问物理内存的功能。
### fileview.c
* 主要功能，能让我们用想要的格式阅读二进制文件。
## Get Started
1. Compile
```bash
make
```
2. install module and device
```bash
make install
```
3. print kernel log
```bash
dmesg
```
4. view /dev/dram
```bash
./fileview /dev/dram
```
5. remove module and device
```bash
make uninstall
```
6. clean up
```bash
make clean
```


## REF
[2.4 动手实践，代码级-把虚拟内存地址转换为物理内存地址_wdh3837的博客-CSDN博客](https://blog.csdn.net/weixin_39247141/article/details/115291539)

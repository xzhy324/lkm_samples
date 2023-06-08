# sys_inspector

A lkm driver that can be used to inspect the system.

## Prerequisites
linux headers for lkm compilation.
for Ubuntu:
```bash
sudo apt-get install linux-headers-$(uname -r)
```

## Usage
```bash
make install
make uninstall
sudo dmesg | tail -n 50
```

## Roadmap
### Features
- [✔] Syscall Table Hooks
- [✔] Hidden Module 
- [✔] Netfilter Hooks
- [✔] Network Protocol VFS Hooks
- [✔] Process VFS Hooks
- [✔] /Proc Zeroed Inodes
### Todo
- [×] Interrupt Descriptor Table Hooks: IDT Descriptors need to be parsed.


## FAQ
Q: when compiling, error occurs at module_mutex. Eg: `note: previous declaration of ‘module_mutex’ was here` \
A: That's because module_mutex is not exported in some versions of kernel.In `<sys_inspector_rrot>/include/config.h`, set `MUTEX_IS_NOT_PREDEFINED` to 0 to use the mutex defined in the kernel header. 
```c
...
#define MUTEX_IS_NOT_PREDEFINED 0
...
```
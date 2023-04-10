clean:
	make -C ./ring0 clean
	make -C ./mylkm clean
	make -C ./pmem clean
	make -C ./sys_inspector clean
	make -C ./rookits/syscall_hooker clean
	make -C ./rookits/module_hidder clean
	make -C ./rookits/netport_hooker clean

.PHONEY: clean
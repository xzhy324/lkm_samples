clean:
	make -C ./ring0 clean
	make -C ./mylkm clean
	make -C ./sys_inspector clean
	make -C ./rookits/syscall_hooker clean
	make -C ./rookits/module_hidder clean
	make -C ./rookits/network_port clean

.PHONEY: clean
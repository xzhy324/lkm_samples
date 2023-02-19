clean:
	make -C ./ring0 clean
	make -C ./mylkm clean
	make -C ./sys_inspector clean
	make -C ./rookits/syscall_hooker clean

.PHONEY: clean
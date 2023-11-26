QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp 1 -machine virt -bios none
GDB = gdb-multiarch

run:
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ./build/$(NAME).elf

.PHONY : debug
debug:
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ./build/$(NAME).elf -s -S &
	@${GDB} ./build/$(NAME).elf -q -x ../gdbinit

fpga:
	python3 /home/axe/WorkSpace/format-conversion/main.py -i $(abspath ./build/$(NAME).bin) -o $(abspath ./build/$(NAME).coe) -f b2c

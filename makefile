comp_debug:
	time scons build/RISCV/gem5.debug -j32 --linker=mold

comp_opt:
	time scons build/RISCV_OPT/gem5.opt -j32 --linker=mold
	cp build/RISCV_OPT/gem5.opt build/RISCV/gem5.opt

comp_fast:
	time scons build/RISCV_FAST/gem5.fast -j32 --linker=mold
	cp build/RISCV_FAST/gem5.fast build/RISCV/gem5.fast

comp_all:
	make comp_debug &
	make comp_opt &
	make comp_fast &

comp_cmd:
	@scons build/RISCV/compile_commands.json
	@echo "===== compile done ====="
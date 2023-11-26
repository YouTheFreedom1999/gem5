CROSS_COMPILE := riscv64-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=rv64ifd -mcmodel=medany
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf64lriscv

START_ADDR=0x80000000
LINK_FILE=$(RISCV64_SDK_RENA_HOME)/riscv64.ld

AM_SRCS := start.S         \
           trm.c           \
           libgcc/muldi3.S \
           libgcc/div.S    \
           ioe.c           \
           timer.c         \
           input.c         \
           cte.c           \
           trap.S          \
           vme.c           \
           mpe.c           \
           uart.c

CFLAGS    += -fdata-sections -ffunction-sections
LDFLAGS   += -T $(LINK_FILE) --defsym=_pmem_start=$(START_ADDR) --defsym=_entry_offset=0x0
# ifneq ($(START_ADDR) , 0x80000000)
# LDFLAGS   += -defsym=_bin_offset=0x1000000
# endif
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS=\"$(mainargs)\"
.PHONY: $(RISCV64_SDK_RENA_HOME)/am/src/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

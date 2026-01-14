CROSS ?= riscv64-linux-gnu-
CC := $(CROSS)gcc
OBJDUMP := $(CROSS)objdump
OBJCOPY := $(CROSS)objcopy

# 仅依赖最小工具链，无 libc/启动文件；禁用 PIE/PIC，避免生成 GOT/动态段
CFLAGS := -march=rv64i -mabi=lp64 -O2 -ffreestanding -fno-builtin -nostdlib -nostartfiles \
	-fno-pic -no-pie -mcmodel=medany -msmall-data-limit=0
LDFLAGS := -T linker.ld -Wl,--gc-sections -static -Wl,-no-dynamic-linker -Wl,-static

all: demo.elf demo.bin demo.dump

demo.elf: start.S main.c linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ start.S main.c

demo.bin: demo.elf
	$(OBJCOPY) -O binary $< $@

demo.dump: demo.elf
	$(OBJDUMP) -d $< > $@

clean:
	rm -f demo.elf demo.bin demo.dump

.PHONY: all clean

CROSS ?= riscv64-linux-gnu-
CC := $(CROSS)gcc
OBJDUMP := $(CROSS)objdump
OBJCOPY := $(CROSS)objcopy

# 仅依赖最小工具链，无 libc/启动文件
CFLAGS := -march=rv64i -mabi=lp64 -O2 -ffreestanding -fno-builtin -nostdlib -nostartfiles
LDFLAGS := -T linker.ld -Wl,--gc-sections

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

CROSS_COMPILE := riscv64-unknown-elf-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

CFLAGS := -march=rv64gc -mabi=lp64d -mcmodel=medany -ffreestanding -nostdlib -O2
LDFLAGS := -T src/linker.ld -nostdlib

all: build/kernel.bin

build/kernel.bin: src/entry.S
	$(CC) $(CFLAGS) -c src/entry.S -o build/entry.o
	$(LD) $(LDFLAGS) build/entry.o -o build/kernel.elf
	$(OBJCOPY) -O binary build/kernel.elf build/kernel.bin

run: build/kernel.bin
	qemu-system-riscv64 \
		-machine virt \
		-nographic \
		-bios none \
		-kernel build/kernel.bin

clean:
	rm -rf build/*

.PHONY: all run clean
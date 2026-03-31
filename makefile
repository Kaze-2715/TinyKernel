CROSS_COMPILE := riscv64-unknown-elf-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

# -Wall: 生成所有警告信息
# -g: 生成调试信息，这样 gdb 可以看到行号和变量信息
CFLAGS := -march=rv64gc -mabi=lp64d -mcmodel=medany -ffreestanding -nostdlib -O2 -Wall -g
LDFLAGS := -T src/linker.ld -nostdlib

SRCS := src/entry.S src/main.c src/uart.c src/printk.c
# 替换引用：将 .c 和 .S 文件替换为 .o 文件
# 语法：$(变量名:模式=替换)，其中模式是 %.c 或 %.S，替换是 %.o
OBJS := $(SRCS:src/%.c=build/%.o)
OBJS := $(OBJS:src/%.S=build/%.o)

all: build/kernel.bin

# @ 的作用是不显示命令本身，只有命令的输出会显示
build/kernel.bin: $(OBJS) src/linker.ld
	@mkdir -p build
	$(LD) $(LDFLAGS) -o build/kernel.elf $(OBJS)
	$(OBJCOPY) -O binary build/kernel.elf build/kernel.bin

build/%.o: src/%.c
	@mkdir -p build
# $< 是自动变量，表示第一个依赖文件，即源文件
# $@ 是自动变量，表示目标文件，即 .o 文件
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: src/%.S
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

run: build/kernel.bin
	qemu-system-riscv64 \
		-machine virt \
		-nographic \
		-bios none \
		-kernel build/kernel.bin

clean:
	rm -rf build src/*.o

.PHONY: all run clean
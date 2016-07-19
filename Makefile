.PHONY: clean force run all

CC=gcc
CFLAGS = -std=gnu99 -Wall -pedantic-errors -m32 -Icore/include -fno-stack-protector -fno-builtin -fno-builtin-function
LFLAGS = -nostdlib -Wl,-Ttext=0x100000,-nostdlib
BUILD_DIR=build

BINARIES=stage1.bin stage2.bin kernel.bin
BIN_FILES = $(addprefix $(BUILD_DIR)/, $(BINARIES))
LIB_FILES = $(wildcard core/lib/*.c)
OBJ_FILES = $(addprefix $(BUILD_DIR)/, $(patsubst core/lib/%.c,%.o,$(LIB_FILES)))

IMAGE = myfloppy.img

all: $(IMAGE)

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE) -boot a -no-fd-bootchk

$(IMAGE): build_dir $(BIN_FILES)
	cat $(BUILD_DIR)/stage1.bin /dev/zero | dd of=$(IMAGE) bs=1024 count=1440
	dcopy $(BUILD_DIR)/stage2.bin $(IMAGE) KRNLDR.SYS
	dcopy $(BUILD_DIR)/kernel.bin $(IMAGE) KERNEL.SYS

build_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -i boot/ $+ -o $@

$(BUILD_DIR)/stage2.bin: boot/stage2/stage2.asm
	nasm -f bin -i boot/ $+ -o $@

$(BUILD_DIR)/%.o: core/lib/%.c
	$(CC) $(CFLAGS) -c $+ -o $@

$(BUILD_DIR)/kernel.bin: core/kernel/main.c core/kernel/entry.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $+ $(LFLAGS) -o $(BUILD_DIR)/kernel.elf
	objcopy -O binary -j .text -j .rodata $(BUILD_DIR)/kernel.elf $@

build: clean all
force: clean all run

clean:
	rm -f $(IMAGE) *.bin kernel *.o
	rm -rf $(BUILD_DIR)

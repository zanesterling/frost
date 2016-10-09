.PHONY: build_dir clean force run all

CC=gcc
OBJCOPY=objcopy
CFLAGS = -g -std=gnu99 -Wall -Wextra -m32 -Icore/include -fno-stack-protector -fno-builtin -fno-builtin-function -masm=intel -pedantic-errors
LFLAGS = -nostdlib -Wl,-Ttext=0x100000,-nostdlib
BUILD_DIR=build
QEMU = qemu-system-i386

ifeq ($(shell uname -s),Darwin)
	CMD_PREFIX=/usr/local/i386-elf-gcc/bin/i386-elf-
	CC = $(CMD_PREFIX)gcc
	OBJCOPY = $(CMD_PREFIX)objcopy
endif

QEMU_FLAGS = -boot a -no-fd-bootchk
ifdef MEMORY_SIZE
	QEMU_FLAGS=$(QEMUFLAGS) -m $(MEMORY_SIZE)
endif

BINARIES=stage1.bin stage2.bin kernel.bin
BIN_FILES = $(addprefix $(BUILD_DIR)/, $(BINARIES))

STAGE1_FILES = $(wildcard boot/stage1/*.asm) $(wildcard boot/stage1/*.inc)
STAGE2_FILES = $(wildcard boot/stage2/*.asm) $(wildcard boot/stage2/*.inc)

MAIN_FILES = core/kernel/entry.c core/kernel/main.c
C_FILES = $(wildcard core/lib/*.c) $(wildcard core/hal/*.c) $(filter-out $(MAIN_FILES),$(wildcard core/kernel/*.c))
OBJ_FILES = $(patsubst core/kernel/%.c,kernel/%.o, $(patsubst core/lib/%.c,%.o, $(patsubst core/hal/%.c,hal/%.o,$(C_FILES))))
OBJ_FILES := $(addprefix $(BUILD_DIR)/,$(OBJ_FILES))

IMAGE = myfloppy.img

all: $(IMAGE)

run: $(IMAGE)
	$(QEMU) -fda $(IMAGE) $(QEMU_FLAGS)

run-debug: $(IMAGE)
	$(QEMU) -fda $(IMAGE) $(QEMU_FLAGS) -s -S

$(IMAGE): build_dir $(BIN_FILES)
	cat $(BUILD_DIR)/stage1.bin /dev/zero | dd of=$(IMAGE) bs=1024 count=1440
	dcopy $(BUILD_DIR)/stage2.bin $(IMAGE) KRNLDR.SYS
	dcopy $(BUILD_DIR)/kernel.bin $(IMAGE) KERNEL.SYS

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/hal
	mkdir -p $(BUILD_DIR)/kernel

$(BUILD_DIR)/stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -i boot/ $+ -o $@

$(BUILD_DIR)/stage2.bin: $(STAGE2_FILES)
	nasm -f bin -i boot/ boot/stage2/stage2.asm -o $@

$(BUILD_DIR)/hal/%.o: core/hal/%.c
	$(CC) $(CFLAGS) -c $+ -o $@

$(BUILD_DIR)/%.o: core/lib/%.c
	$(CC) $(CFLAGS) -c $+ -o $@

$(BUILD_DIR)/kernel/%.o: core/kernel/%.c
	$(CC) $(CFLAGS) -c $+ -o $@

$(BUILD_DIR)/kernel.bin: $(MAIN_FILES) $(OBJ_FILES)
	$(CC) $(CFLAGS) $+ $(LFLAGS) -o $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary -j .text -j .data -j .rodata $(BUILD_DIR)/kernel.elf $@

build: clean all
force: clean all run

clean:
	rm -f $(IMAGE) *.bin kernel *.o
	rm -rf $(BUILD_DIR)

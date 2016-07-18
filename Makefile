.PHONY: clean force run all

CC=gcc
CFLAGS = -Wall -pedantic-errors -m32 -Icore/include
LFLAGS = -nostdlib -Wl,-Ttext=0x100000,-nostdlib

all: myfloppy.img

run: myfloppy.img
	qemu-system-i386 -fda myfloppy.img -boot a -no-fd-bootchk

myfloppy.img: stage1.bin stage2.bin kernel.bin
	cat stage1.bin /dev/zero | dd of=myfloppy.img bs=1024 count=1440
	dcopy stage2.bin myfloppy.img KRNLDR.SYS
	dcopy kernel.bin myfloppy.img KERNEL.SYS

stage1.bin: boot/stage1/stage1.asm
	nasm -f bin -i boot/ $+ -o $@

stage2.bin: boot/stage2/stage2.asm
	nasm -f bin -i boot/ $+ -o $@

kernel.bin: core/kernel/main.c core/kernel/entry.c #core/kernel/kernel_stub.asm
	$(CC) $(CFLAGS) -c core/lib/io.c
	$(CC) $(CFLAGS) $+ io.o $(LFLAGS) -o kernel
	objcopy -O binary -j .text -j .rodata kernel $@
	#nasm -f bin $+ -o $@

build: clean all
force: clean all run

clean:
	rm -f myfloppy.img *.bin kernel *.o

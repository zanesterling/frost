.PHONY: clean force run all

all: myfloppy.img

run: myfloppy.img
	qemu-system-i386 -fda myfloppy.img -boot a -no-fd-bootchk

myfloppy.img: stage1.bin stage2.bin kernel.bin
	cat stage1.bin /dev/zero | dd of=myfloppy.img bs=1024 count=1440
	dcopy stage2.bin myfloppy.img KRNLDR.SYS
	dcopy kernel.bin myfloppy.img KERNEL.SYS

stage1.bin: stage1/stage1.asm
	nasm -f bin $+ -o $@

stage2.bin: stage2/stage2.asm
	nasm -f bin $+ -o $@

kernel.bin: kernel/kernel_stub.asm
	nasm -f bin $+ -o $@

force: clean all run

clean:
	rm -f myfloppy.img *.bin

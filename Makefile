.PHONY: clean force all

all: myfloppy.img

myfloppy.img: boot1.bin stage2.bin
	cat boot1.bin /dev/zero | dd of=myfloppy.img bs=1024 count=1440
	dcopy stage2.bin myfloppy.img KRNLDR.SYS

boot1.bin: boot1.asm
	nasm -f bin boot1.asm -o boot1.bin

stage2.bin: stage2.asm
	nasm -f bin stage2.asm -o stage2.bin

force: clean all

clean:
	rm -f myfloppy.img *.bin

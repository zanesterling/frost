/*
 * entry.c
 * This is the kernel entry point.
 * Called from the bootloader.
 */

#if !(defined(__i386) || defined(_M_IX86))
#error Only x86 is supported.
#endif

#include <bootinfo.h>

extern int kernel_main(multiboot_info* info);

void kernel_entry(multiboot_info* bootinfo) {
	asm(
		/* set data segments to data selector */
		"mov %ax, 0x10\n"
		"mov %ds, %ax\n"
		"mov %es, %ax\n"
		"mov %fs, %ax\n"
		"mov %gs, %ax\n"
	);

	kernel_main(bootinfo);

	/* stall to avoid returning to nowhere */
	for (;;);
}

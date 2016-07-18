/*
 * entry.c
 * This is the kernel entry point.
 * Called from the bootloader.
 */

#if !(defined(__i386) || defined(_M_IX86))
#error Only x86 is supported.
#endif

extern void main();

void kernel_entry() {
	asm(
		/* set data segments to data selector */
		"mov %ax, 0x10\n"
		"mov %ds, %ax\n"
		"mov %es, %ax\n"
		"mov %fs, %ax\n"
		"mov %gs, %ax\n"
		"mov %ss, %ax\n"
		"mov %esp, 0x90000\n" /* stack begins from 90000h */
		"mov %ebp, %esp\n"
		"push %ebp\n"
	);

	main();

	/* stall to avoid returning to nowhere */
	for (;;);
}

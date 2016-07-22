#include "hal.h"

int hal_initialize() {
	int err = i86_gdt_initialize();
	if (err) return err;
	err = i86_idt_initialize(0x8);
	if (err) return err;
	return 0;
}

int hal_shutdown() {
	return 0;
}

void geninterrupt(uint8_t n) {
	asm(
		"mov genint%=, %%ebx\n"
		"movb %%al, 1(%%ebx)\n"
		"genint%=:\n"
		"int $0\n"
		:: "a" (n)
	);
}

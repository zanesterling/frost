#include "hal.h"

int hal_initialize() {
	int err = i86_gdt_initialize();
	if (err) return err;

	err = i86_idt_initialize(0x8);
	if (err) return err;

	err = i86_pic_initialize();
	if (err) return err;

	return 0;
}

int hal_shutdown() {
	return 0;
}

void geninterrupt(uint8_t n) {
	asm(
		"lea eax, genint%=\n"
		"movb [eax+1], bl\n"
		"genint%=:\n"
		"int 0\n"
		:: "b" (n)
	);
}

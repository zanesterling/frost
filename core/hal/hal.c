#include "hal.h"

#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "pit.h"

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

void setvect(uint8_t interrupt, void (*vect)()) {
	i86_install_ir(
		interrupt,
		I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
		8,
		vect
	);
}

inline void interruptdone(uint8_t interrupt) {
	if (interrupt > 16) return;
	uint8_t picNum = (interrupt >= 8) ? 1 : 0;
	i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, picNum);
}

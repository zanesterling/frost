#include "idt.h"

/* Private stuff */

#pragma pack(push, 1)

struct idtr {
	uint16_t limit;
	uint32_t base;
};

#pragma pack(pop)

static struct idt_descriptor _idt[I86_MAX_INTERRUPTS];
static struct idtr _idtr;

static void idt_install() {
	asm("lidt (%0)" :: "r" (&_idtr));
}

/* Public stuff */
/* Interrupt handlers */

void i86_default_handler() {
	puts("*** [i86 Hal] i86_default_handler: Unhandled Exception");
	for (;;);
}

int i86_install_ir(
	uint8_t ir_num, uint16_t flags, uint16_t selector, I86_IRQ_HANDLER irq
) {
	if (!irq) return -1;

	uint32_t uiBase = (uint32_t)irq;

	_idt[ir_num].baseLow  = uiBase & 0xffff;
	_idt[ir_num].baseHigh = (uiBase >> 16) & 0xffff;
	_idt[ir_num].reserved = 0;
	_idt[ir_num].flags = flags;
	_idt[ir_num].selector = selector;

	return 0;
}

int i86_idt_initialize(uint16_t code_selector) {
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base = (uint32_t)_idt;
	memset(_idt, 0, _idtr.limit);

	for (int i = 0; i < I86_MAX_INTERRUPTS; i++) {
		int ret = i86_install_ir(
			i,
			I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			code_selector,
			(I86_IRQ_HANDLER) i86_default_handler
		);
		if (ret) return ret;
	}

	idt_install();
	return 0;
}

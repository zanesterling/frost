#include <hal.h>

#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "pit.h"

int hal_initialize() {
	int err = i86_gdt_initialize();
	if (err) return err;

	err = i86_idt_initialize(0x8);
	if (err) return err;

	i86_pic_initialize();
	i86_pit_initialize();

	return 0;
}

int hal_shutdown() {
	return 0;
}

void enable() { asm("sti\n"); }
void disable() { asm("cli\n"); }

void geninterrupt(uint8_t n) {
	asm(
		"lea eax, genint%=\n"
		"movb [eax+1], bl\n"
		"genint%=:\n"
		"int 0\n"
		:: "b" (n)
	);
}

uint8 inbyte(uint16_t port) {
	uint8 ret;
	asm("inb %0, %1" : "=a" (ret) : "d" (port));
	return ret;
}

void outbyte(uint16_t port, uint8_t data) {
	asm("outb %0, %1" : : "d" (port), "a" (data));
}

void setvect(uint8_t interrupt, void (*vect)()) {
	i86_install_ir(
		interrupt,
		I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
		8,
		vect
	);
}

I86_IRQ_HANDLER getvect(uint8_t interrupt) {
	struct idt_descriptor desc = i86_get_descriptor(interrupt);
	return (I86_IRQ_HANDLER)(desc.baseLow + (desc.baseHigh << 16));
}

inline void interruptdone(uint8_t interrupt) {
	if (interrupt > 16) return;
	uint8_t picNum = (interrupt >= 8) ? 1 : 0;
	i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, picNum);
}

void sound(uint8_t freq) {
	outbyte(0x61, 3 | ((uint8_t) freq << 2));
}

char vendor[32];
const char* get_cpu_vendor() {
	asm(
		"mov eax, 0\n"
		"cpuid\n"
		"mov dword [[vendor]], ebx\n"
		"mov dword [[vendor]+4], ecx\n"
		"mov dword [[vendor]+8], edx\n"
		:: [vendor] "X" (vendor)
		: "eax","ebx","ecx","edx"
	);

	return vendor;
}

uint32_t get_tick_count() {
	return i86_get_counter();
}

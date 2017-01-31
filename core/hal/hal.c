#include <hal.h>

#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "pit.h"

/* PRIVATE DATA */
multiboot_info* _bootinfo = 0;

uint32_t _sleepers[256];


/* PUBLIC FUNCTION IMPLS */

multiboot_info* get_bootinfo() { return _bootinfo; }

int hal_initialize(multiboot_info* bootinfo) {
	printf("_sleepers: %#08x\n", _sleepers);
	int err = i86_gdt_initialize();
	if (err) return err;

	err = i86_idt_initialize(0x8);
	if (err) return err;

	i86_pic_initialize();
	i86_pit_initialize();
	i86_pit_start_counter(
		100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN
	);

	_bootinfo = bootinfo;

	memset(_sleepers, 0, sizeof(_sleepers));

	return 0;
}

int hal_shutdown() {
	return 0;
}

void enable_interrupts() { asm("sti\n"); }
void disable_interrupts() { asm("cli\n"); }

void geninterrupt(uint8_t n) {
	asm(
		"lea eax, genint%=\n"
		"movb [eax+1], bl\n"
		"genint%=:\n"
		"int 0\n"
		:: "b" (n)
	);
}

void register_timer_interrupt(pit_handler handler) {
	pit_set_timer_handler(handler);
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

void interruptdone(uint8_t interrupt) {
	if (interrupt <= 16) {
		uint8_t picNum = (interrupt >= 8) ? 1 : 0;
		i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, picNum);
	}

	// if there's a sleeper waiting, wake him up
	if (false && ((void*)_sleepers[interrupt] != NULL)) {
		asm(
			"mov dword [ebp + 4], %0\n"
			:: "r" (_sleepers[interrupt])
		);
	}
}

void irq_wait(uint8_t interrupt) {
	asm("cli\n");

	if ((void*)_sleepers[interrupt] != NULL) {
		puts("irq_wait: tried to wait on an irq being waited on");
		asm("cli\nhlt\n");
	}

	// save post-loop return addr in sleepr table
	//asm("mov %0, awaken\n" : "=r" (_sleepers[interrupt]));

	asm(
		"sti\n"
		"sleep: hlt\n"
		"jmp sleep\n"
		"awaken: nop\n"
	);
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

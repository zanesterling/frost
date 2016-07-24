#pragma once

#include "io.h"
#include "stdint.h"

#define I86_MAX_INTERRUPTS 256

#define I86_IDT_DESC_BIT16   0x06	//00000110
#define I86_IDT_DESC_BIT32   0x0E	//00001110
#define I86_IDT_DESC_RING1   0x40	//01000000
#define I86_IDT_DESC_RING2   0x20	//00100000
#define I86_IDT_DESC_RING3   0x60	//01100000
#define I86_IDT_DESC_PRESENT 0x80	//10000000

typedef void(* I86_IRQ_HANDLER)(void);

#pragma pack(push, 1)

struct idt_descriptor {
	uint16_t baseLow;
	uint16_t selector;
	uint8_t reserved; // should be zero
	uint8_t flags;
	uint16_t baseHigh;
} idt_descriptor;

#pragma pack(pop)

int i86_install_ir(uint8_t ir_num, uint16_t flags, uint16_t selector, I86_IRQ_HANDLER irq);
int i86_idt_initialize(uint16_t code_selector);

#pragma once

#include <stdint.h>

// PIC ports
#define I86_PIC_PORT_COMMAND     0x20
#define I86_PIC_PORT_STATUS_REG  0x20
#define I86_PIC_PORT_MASK_REG    0x21
#define I86_PIC_PORT_DATA_REG    0x21
#define I86_PIC2_PORT_COMMAND    0xA0
#define I86_PIC2_PORT_STATUS_REG 0xA0
#define I86_PIC2_PORT_MASK_REG   0xA1
#define I86_PIC2_PORT_DATA_REG   0xA1

// PIC1 devices
#define I86_PIC_IRQ_TIMER      0
#define I86_PIC_IRQ_KEYBOARD   1
#define I86_PIC_IRQ_SERIAL2    3
#define I86_PIC_IRQ_SERIAL1    4
#define I86_PIC_IRQ_PARALLEL2  5
#define I86_PIC_IRQ_DISKETTE   6
#define I86_PIC_IRQ_PARALLEL1  7

// PIC2 devices
#define I86_PIC_IRQ_CMOSTIMER  0
#define I86_PIC_IRQ_CGARETRACE 1
#define I86_PIC_IRQ_AUXILIARY  4
#define I86_PIC_IRQ_FPU        5
#define I86_PIC_IRQ_HDC        6

// Command Word 2 bit masks. Use when sending commands
#define		I86_PIC_OCW2_MASK_L1		1		//00000001
#define		I86_PIC_OCW2_MASK_L2		2		//00000010
#define		I86_PIC_OCW2_MASK_L3		4		//00000100
#define		I86_PIC_OCW2_MASK_EOI		0x20	//00100000
#define		I86_PIC_OCW2_MASK_SL		0x40	//01000000
#define		I86_PIC_OCW2_MASK_ROTATE	0x80	//10000000

//! Command Word 3 bit masks. Use when sending commands
#define		I86_PIC_OCW3_MASK_RIS		1		//00000001
#define		I86_PIC_OCW3_MASK_RIR		2		//00000010
#define		I86_PIC_OCW3_MASK_MODE		4		//00000100
#define		I86_PIC_OCW3_MASK_SMM		0x20	//00100000
#define		I86_PIC_OCW3_MASK_ESMM		0x40	//01000000
#define		I86_PIC_OCW3_MASK_D7		0x80	//10000000

void i86_pic_initialize();
void i86_pic_send_command(uint8_t cmd, uint8_t pic);
void i86_pic_send_data(uint8_t data, uint8_t pic);
int16_t i86_pic_read_data(uint8_t pic);
void sendoi();

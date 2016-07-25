#pragma once

#include "asm.h"
#include "idt.h"
#include "stdint.h"

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

int i86_pic_initialize();
void sendoi();

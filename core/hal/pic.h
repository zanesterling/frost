#pragma once

#include "asm.h"
#include "idt.h"
#include "stdint.h"

#define PIC_PORT_COMMAND     0x20
#define PIC_PORT_STATUS_REG  0x20
#define PIC_PORT_MASK_REG    0x21
#define PIC_PORT_DATA_REG    0x21
#define PIC2_PORT_COMMAND    0xA0
#define PIC2_PORT_STATUS_REG 0xA0
#define PIC2_PORT_MASK_REG   0xA1
#define PIC2_PORT_DATA_REG   0xA1

int i86_pic_initialize();
void sendoi();

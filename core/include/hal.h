#pragma once

#include "../hal/idt.h"
#include "../hal/gdt.h"
#include "../hal/pic.h"

extern int hal_initialize();
extern int hal_shutdown();
extern void geninterrupt(uint8_t n);

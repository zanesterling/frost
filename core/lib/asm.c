#include "asm.h"

uint8 inbyte(uint16 port) {
	uint8 ret;
	asm("inb %0, %1" : "=a" (ret) : "d" (port));
	return ret;
}

void outbyte(uint16 port, uint8 data) {
	asm("outb %0, %1" : : "d" (port), "a" (data));
}

#include "asm.h"

unsigned char inbyte(unsigned short port) {
	unsigned char ret;
	asm("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

void outbyte(unsigned short port, unsigned char data) {
	asm("outb %1, %0" : : "d" (port), "a" (data));
}

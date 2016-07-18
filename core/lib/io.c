#include "io.h"

void Putch(const char c) {
	if (c != '\n') {
		unsigned char* p = (unsigned char*)VID_MEM;
		p += 2 * (_CurY * COLS + _CurX++);
		*p++ = c;
		*p = CHAR_ATTRIB;
	}

	if (c == '\n' || _CurX == COLS) {
		_CurX = 0;
		++_CurY;
	}
}

void Puts(const char* str) {
	for (; *str != 0; ++str) {
		Putch(*str);
	}

	MoveCursor();
}

void MoveCursor() {
	unsigned short val = _CurY * COLS + _CurX;
	outbyte(0x03d4, 0x0f);
	outbyte(0x03d5, val & 0xff);
	outbyte(0x03d4, 0x0e);
	outbyte(0x03d5, (val >> 8) & 0xff);
}

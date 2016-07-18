#include "io.h"

void Putch(const char c) {
	if (_CurY == ROWS) {
		scroll(1);
	}

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

void scroll(const uint8 numRows) {
	_CurY = _CurY < numRows ? 0 : _CurY - numRows;
	uint16* offset = (uint16*)VID_MEM;
	offset += numRows * COLS;
	memcpy((uint16*)VID_MEM, offset, VID_MEM_MAX - VID_MEM - 2 * numRows * COLS);
}

void Puts(const char* str) {
	for (; *str != 0; ++str) {
		Putch(*str);
	}

	moveCursor();
}

void moveCursor() {
	unsigned short val = _CurY * COLS + _CurX;
	outbyte(0x03d4, 0x0f);
	outbyte(0x03d5, val & 0xff);
	outbyte(0x03d4, 0x0e);
	outbyte(0x03d5, (val >> 8) & 0xff);
}

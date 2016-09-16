#include "io.h"

/* Private stuff */
static const char base_10_chars[10] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
static const char hex_chars[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

/* Public stuff */
void putch(const char c) {
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
	update_cursor();
}

void scroll(const uint8 numRows) {
	_CurY = _CurY < numRows ? 0 : _CurY - numRows;
	uint16* offset = (uint16*)VID_MEM;
	offset += numRows * COLS;
	memcpy((uint16*)VID_MEM, offset, VID_MEM_MAX - VID_MEM - 2 * numRows * COLS);
}

void puts(const char* str) {
	for (; *str != 0; ++str) {
		putch(*str);
	}

	update_cursor();
}

void printf(const char* fmt, ...) {
	const char *p;
	int x;
	char* str;
	char buf[32];
	va_list argp;
	va_start(argp, fmt);


	for (p = fmt; *p; p++) {
		if (*p != '%') {
			putch(*p);
			continue;
		}

		switch (*++p) {
			case 'c':
				x = va_arg(argp, int);
				putch(x);
				break;

			case 'd':
			case 'i':
				x = va_arg(argp, int);
				itoa(x, buf);
				puts(buf);
				break;

			case 'x':
				x = va_arg(argp, int);
				itoa_s(x, buf, 16, hex_chars);
				puts(buf);
				break;

			case 's':
				str = va_arg(argp, char*);
				while (*str) putch(*str++);
				break;

			case '%':
				putch('%');
				break;
		}
	}
}

void clearScreen() {
	uint16_t* p = (uint16_t*) VID_MEM;
	for (size_t i = 0; i < COLS * ROWS; i++) {
		*p++ = 0;
	}

	_CurX = 0;
	_CurY = 0;
	update_cursor();
}

void move_cursor(uint8_t x, uint8_t y) {
	_CurX = x;
	_CurY = y;
}

struct cursor_loc get_cursor_loc() {
	struct cursor_loc ret = {_CurX, _CurY};
	return ret;
}

void update_cursor() {
	unsigned short val = _CurY * COLS + _CurX;
	outbyte(0x03d4, 0x0f);
	outbyte(0x03d5, val & 0xff);
	outbyte(0x03d4, 0x0e);
	outbyte(0x03d5, (val >> 8) & 0xff);
}

/* UTILITIES */

void itoa(int x, char* buf) {
	itoa_s(x, buf, 10, base_10_chars);
}

void itoa_s(int x, char* buf, const size_t base, const char* base_chars) {
	if (x == 0) {
		buf[0] = base_chars[0];
		buf[1] = 0;
		return;
	}

	uint8 len = 0;
	while (x > 0) {
		buf[len++] = base_chars[x % base];
		x /= base;
	}

	/* int gets put in backwards, reverse it */
	for (uint8 i = 0; i < len / 2; i++) {
		char tmp = buf[i];
		buf[i] = buf[len-1 - i];
		buf[len-1 - i] = tmp;
	}
	buf[len] = 0;
}

/* INPUT */

char getch() {
	KEYCODE key = KEY_UNKNOWN;
	while (key == KEY_UNKNOWN) key = kbrd_get_last_key();
	kbrd_discard_last_key();
	return kbrd_key_to_ascii(key);
}

uint8 getScancode() {
	while (1) {
		uint8 c = inbyte(0x60);
		if (c) return c;
	}
}

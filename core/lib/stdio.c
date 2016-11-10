#include "stdio.h"

/* Private stuff */
uint8_t _CurX, _CurY;

static const char base_10_chars[10] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
static const char hex_chars[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

#define STEP_CURSOR() _CurX++; \
	if (_CurX == COLS) { \
		_CurX = 0; \
		_CurY++; \
	}

void _raw_putch(const char c);


/* Public stuff */
void putch(const char c) {
	if (_CurY == ROWS) {
		scroll(1);
	}

	if (c == '\n') {
		while (_CurX < COLS) {
			_raw_putch(' ');
			_CurX++;
		}

		_CurX = 0;
		++_CurY;
	} else if (c == '\t') {
		do {
			_raw_putch(' ');
			STEP_CURSOR();
		} while (_CurX % TABWIDTH != 0);
	} else {
		unsigned char* p = (unsigned char*)VID_MEM;
		p += 2 * (_CurY * COLS + _CurX);
		*p++ = c;
		*p = CHAR_ATTRIB;

		STEP_CURSOR();
	}
	update_cursor();
}

void _raw_putch(const char c) {
	uint8_t* p = (uint8_t*)VID_MEM + 2 * (_CurY * COLS + _CurX);
	*p = c;
	*(p+1) = CHAR_ATTRIB;
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

		uint8_t length = 2;

		uint8_t done = 0;
		while (!done) {
			switch (*++p) {
				case 'l':
					length++;
					break;

				case 'c':
					x = va_arg(argp, int);
					putch(x);
					done = 1;
					break;

				case 'd':
				case 'i':
					if (length == 0) x = va_arg(argp, char);
					else if (length == 1) x = va_arg(argp, short);
					else if (length == 2) x = va_arg(argp, int);
					else if (length == 3) x = va_arg(argp, long);
					else if (length == 4) x = va_arg(argp, long long);
					itoa(x, buf);
					puts(buf);
					done = 1;
					break;

				case 'u':
					if (length == 0) x = va_arg(argp, unsigned char);
					else if (length == 1) x = va_arg(argp, unsigned short);
					else if (length == 2) x = va_arg(argp, unsigned int);
					else if (length == 3) x = va_arg(argp, unsigned long);
					else if (length == 4) x = va_arg(argp, unsigned long long);
					itoa_unsigned(x, buf);
					puts(buf);
					done = 1;
					break;

				case 'x':
					if (length == 0) x = va_arg(argp, char);
					else if (length == 1) x = va_arg(argp, short);
					else if (length == 2) x = va_arg(argp, int);
					else if (length == 3) x = va_arg(argp, long);
					else if (length == 4) x = va_arg(argp, long long);
					itoa_s_unsigned(x, buf, 16, hex_chars);
					puts(buf);
					done = 1;
					break;

				case 's':
					str = va_arg(argp, char*);
					while (*str) putch(*str++);
					done = 1;
					break;

				case '%':
					putch('%');
					done = 1;
					break;
			}
		}
	}
}

void clear_screen() {
	uint16_t* p = (uint16_t*) VID_MEM;
	for (size_t i = 0; i < COLS * ROWS; i++) {
		*p++ = 0;
	}

	_CurX = 0;
	_CurY = 0;
	update_cursor();
}

void clear_rest_of_row() {
	uint16_t* p = ((uint16_t*) VID_MEM) + _CurY * COLS + _CurX;
	for (size_t i = _CurX; i < COLS; i++) {
		*p++ = 0;
	}
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

void itoa(int64_t x, char* buf) {
	itoa_s(x, buf, 10, base_10_chars);
}

void itoa_unsigned(uint64_t x, char* buf) {
	itoa_s_unsigned(x, buf, 10, base_10_chars);
}

void itoa_s(int64_t x, char* buf, const uint8_t base, const char* base_chars) {
	if (x == 0) {
		buf[0] = base_chars[0];
		buf[1] = '\0';
		return;
	}

	bool negative = x < 0;
	if (negative) x = -x;

	uint8_t len = 0;
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
	buf[len] = '\0';

	if (negative) {
		for (int i = len + 1; i > 0; i--) {
			buf[i] = buf[i - 1];
		}
		buf[0] = '-';
	}
}

void itoa_s_unsigned(
	uint64_t x, char* buf, const uint8_t base, const char* base_chars
) {
	if (x == 0) {
		buf[0] = base_chars[0];
		buf[1] = '\0';
		return;
	}

	uint8_t len = 0;
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
	buf[len] = '\0';
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

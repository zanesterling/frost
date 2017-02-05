#include "stdio.h"

/* Private includes */
#include "keyboard.h"
#include "hal.h"
#include "scancodes.h"
#include "stdarg.h"
#include "string.h"

/* Private stuff */
enum printf_mode {
	MODE_FLAG,
	MODE_FIELD_WIDTH,
	MODE_PRECISION,
	MODE_FORMAT,
	MODE_DONE,
};

uint8_t _CurX = 0, _CurY = 0;
uint8_t _buffer[VID_MEM_MAX - VID_MEM] = {0};
enum render_mode _render_mode = RENDER_MODE_DIRECT;

static const char base_10_chars[10] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
static const char HEX_CHARS[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
static const char* OCTAL_CHARS = HEX_CHARS;

#define STEP_CURSOR() _CurX++; \
	if (_CurX == COLS) { \
		_CurX = 0; \
		_CurY++; \
	}

#define GET_BUFFER() ((_render_mode == RENDER_MODE_DIRECT) ? (uint8_t*)VID_MEM : _buffer)

void _raw_putch(const char c);
void _raw_putch_at(const char c, const uint8_t color, const uint8_t x, const uint8_t y);

const char* _printf_do_escape(const char*, va_list*);

/* Public stuff */
void init_rendering() {
	set_render_mode(RENDER_MODE_DIRECT);
	move_cursor(0, 0);
}

void putch(const char c) {
	if (_CurY >= ROWS) {
		scroll(_CurY - ROWS + 1);
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
		_raw_putch(c);
		STEP_CURSOR();
	}
	update_cursor();
}

void draw_char_at(
	char c,
	uint8_t fore,
	uint8_t back,
	int x,
	int y
) {
	_raw_putch_at(c, fore | (back << 4), x, y);
}

void _raw_putch(const char c) { _raw_putch_at(c, CHAR_ATTRIB, _CurX, _CurY); }
void _raw_putch_at(const char c, uint8_t color, uint8_t x, uint8_t y) {
	uint8_t* p = GET_BUFFER() + 2 * (y * COLS + x);
	*p = c;
	*(p+1) = color;
}

void scroll(const uint8 numRows) {
	_CurY = _CurY < numRows ? 0 : _CurY - numRows;
	uint8_t* offset = (uint8_t*)VID_MEM + 2 * numRows * COLS;
	size_t bytes_to_copy = VID_MEM_MAX - VID_MEM - 2 * numRows * COLS;
	memcpy((void*)VID_MEM, offset, bytes_to_copy);
	memset((uint8_t*)VID_MEM + bytes_to_copy, 0, (VID_MEM_MAX - VID_MEM) - bytes_to_copy);
}

void puts(const char* str) {
	for (; *str != 0; ++str) {
		putch(*str);
	}

	update_cursor();
}

void printf(const char* fmt, ...) {
	const char *p;
	va_list argp;
	va_start(argp, fmt);

	for (p = fmt; *p; p++) {
		if (*p != '%') {
			putch(*p);
			continue;
		}

		p = _printf_do_escape(p, &argp);
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

int itoa(int64_t x, char* buf) {
	return itoa_s(x, buf, 10, base_10_chars);
}

int itoa_unsigned(uint64_t x, char* buf) {
	return itoa_s_unsigned(x, buf, 10, base_10_chars);
}

int itoa_hex(uint64_t x, char* buf) {
	return itoa_s_unsigned(x, buf, 16, HEX_CHARS);
}

int itoa_s(
	int64_t x,
	char* buf,
	const uint8_t base,
	const char* base_chars
) {
	if (x == 0) {
		buf[0] = base_chars[0];
		buf[1] = '\0';
		return 1;
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

	return len;
}

int itoa_s_unsigned(
	uint64_t x, char* buf, const uint8_t base, const char* base_chars
) {
	if (x == 0) {
		buf[0] = base_chars[0];
		buf[1] = '\0';
		return 1;
	}

	uint8_t len = 0;
	while (x > 0) {
		buf[len++] = base_chars[x % base];
		x /= base;
	}

	/* int gets put in backwards, reverse it */
	for (uint8_t i = 0; i < len / 2; i++) {
		char tmp = buf[i];
		buf[i] = buf[len-1 - i];
		buf[len-1 - i] = tmp;
	}
	buf[len] = '\0';

	return len;
}

/* INPUT */

char getch() {
	KEYCODE key = KEY_UNKNOWN;
	while (key == KEY_UNKNOWN) key = kbrd_get_last_key();
	kbrd_discard_last_key();
	return kbrd_key_to_ascii(key);
}

int getch_nonblocking() {
	KEYCODE key = kbrd_get_last_key();
	if (key == KEY_UNKNOWN) return -1;
	kbrd_discard_last_key();
	return kbrd_key_to_ascii(key);
}

uint8 getScancode() {
	while (1) {
		uint8 c = inbyte(0x60);
		if (c) return c;
	}
}


/* PRIVATE IMPLS */

const char* _printf_do_escape(const char* p, va_list* argp) {
	// p should point at the start of the format escape
	if (*p != '%') return p;

	int64_t x;
	char buf[32];
	char* str;

	uint8_t length = 2;
	enum printf_mode mode = MODE_FLAG;
	char preface_character = ' ';
	int field_width = 0;
	int precision = 0;
	bool alternate_output = false;

	int output_length;
	while (mode != MODE_DONE) {
		// handle field width / precision parsing
		char c = *++p;
		if ('1' <= c && c <= '9') {
			if (mode == MODE_FLAG) {
				mode = MODE_FIELD_WIDTH;
				field_width = c - '0';
			} else if (mode == MODE_FIELD_WIDTH) {
				field_width = field_width * 10 + (c - '0');
			} else if (mode == MODE_PRECISION) {
				precision = precision * 10 + (c - '0');
			}
			continue;
		}

		// handle flags, output types
		switch (c) {
			case '0':
				if (mode == MODE_FLAG) {
					preface_character = '0';
				} else if (mode == MODE_FIELD_WIDTH) {
					field_width = field_width * 10;
				} else if (mode == MODE_PRECISION) {
					precision = precision * 10;
				}
				break;

			case '#':
				alternate_output = true;
				break;

			case 'l':
				length++;
				break;

			case 'c':
				x = va_arg(*argp, int);
				putch(x);
				mode = MODE_DONE;
				break;

			case 'd':
			case 'i':
				if (length == 0) x = va_arg(*argp, char);
				else if (length == 1) x = va_arg(*argp, short);
				else if (length == 2) x = va_arg(*argp, int);
				else if (length == 3) x = va_arg(*argp, long);
				else if (length == 4) x = va_arg(*argp, long long);
				output_length = itoa(x, buf);
				for (; field_width > output_length; field_width--) {
					putch(preface_character);
				}
				puts(buf);
				mode = MODE_DONE;
				break;

			case 'u':
				if (length == 0) x = va_arg(*argp, unsigned char);
				else if (length == 1) x = va_arg(*argp, unsigned short);
				else if (length == 2) x = va_arg(*argp, unsigned int);
				else if (length == 3) x = va_arg(*argp, unsigned long);
				else if (length == 4) x = va_arg(*argp, unsigned long long);
				output_length = itoa_unsigned(x, buf);
				for (; field_width > output_length; field_width--) {
					putch(preface_character);
				}
				puts(buf);
				mode = MODE_DONE;
				break;

			case 'o':
				if (length == 0) x = va_arg(*argp, char);
				else if (length == 1) x = va_arg(*argp, short);
				else if (length == 2) x = va_arg(*argp, int);
				else if (length == 3) x = va_arg(*argp, long);
				else if (length == 4) x = va_arg(*argp, long long);
				if (alternate_output) {
					field_width -= 1;
					putch('0');
				}
				output_length = itoa_s_unsigned(x, buf, 8, OCTAL_CHARS);
				for (; field_width > output_length; field_width--) {
					putch(preface_character);
				}
				puts(buf);
				mode = MODE_DONE;
				break;

			case 'x':
				if (length == 0) x = va_arg(*argp, unsigned char);
				else if (length == 1) x = va_arg(*argp, unsigned short);
				else if (length == 2) x = va_arg(*argp, unsigned int);
				else if (length == 3) x = va_arg(*argp, unsigned long);
				else if (length == 4) x = va_arg(*argp, unsigned long long);
				if (alternate_output) {
					field_width -= 2;
					puts("0x");
				}
				output_length = itoa_s_unsigned(x, buf, 16, HEX_CHARS);
				for (; field_width > output_length; field_width--) {
					putch(preface_character);
				}
				puts(buf);
				mode = MODE_DONE;
				break;

			case 's':
				str = va_arg(*argp, char*);
				while (*str) putch(*str++);
				mode = MODE_DONE;
				break;

			case '%':
				putch('%');
				mode = MODE_DONE;
				break;
		}
	}

	return p;
}

void set_render_mode(enum render_mode mode) { _render_mode = mode; }
void display() { memcpy((void*)VID_MEM, _buffer, VID_MEM_MAX - VID_MEM); }

#pragma once

#include "hal.h"
#include "keyboard.h"
#include "scancodes.h"
#include "stdarg.h"
#include "string.h"

#define VID_MEM 0xB8000
#define COLS 80
#define ROWS 25
#define VID_MEM_MAX (VID_MEM + COLS * ROWS * 2)
#define CHAR_ATTRIB 15 /* white on black */
#define TABWIDTH 4

static uint8_t _CurX, _CurY;

struct cursor_loc {
	uint8_t x, y;
};

void putch(const char c);
void scroll(const uint8 numRows);
void puts(const char* str);
void printf(const char* fmt, ...);
void move_cursor(uint8_t x, uint8_t y);
struct cursor_loc get_cursor_loc();
void update_cursor();
void clear_screen();
void clear_rest_of_row();

void itoa(const int64_t x, char* buf);
void itoa_unsigned(const uint64_t x, char* buf);
void itoa_s(const int64_t x, char* buf, const uint8_t base, const char* base_chars);
void itoa_s_unsigned(
	const uint64_t x, char* buf, const uint8_t base, const char* base_chars
);

char getch();
uint8 getScancode();

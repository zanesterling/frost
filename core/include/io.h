#pragma once

#include "hal.h"
#include "scancodes.h"
#include "stdarg.h"
#include "string.h"

#define VID_MEM 0xB8000
#define COLS 80
#define ROWS 25
#define VID_MEM_MAX (VID_MEM + COLS * ROWS * 2)
#define CHAR_ATTRIB 15 /* white on black */

uint8_t _CurX, _CurY;

void putch(const char c);
void scroll(const uint8 numRows);
void puts(const char* str);
void printf(const char* fmt, ...);
void move_cursor(uint8_t x, uint8_t y);
void update_cursor();
void clearScreen();

void itoa(const int x, char* buf);
void itoa_s(const int x, char* buf, const size_t base, const char* base_chars);

char getch();
uint8 getScancode();

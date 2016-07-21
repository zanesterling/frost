#pragma once

#include "asm.h"
#include "scancodes.h"
#include "stdarg.h"
#include "string.h"

#define VID_MEM 0xB8000
#define COLS 80
#define ROWS 25
#define VID_MEM_MAX (VID_MEM + COLS * ROWS * 2)
#define CHAR_ATTRIB 15 /* white on black */

unsigned char _CurX, _CurY;

void putch(const char c);
void scroll(const uint8 numRows);
void puts(const char* str);
void printf(const char* fmt, ...);
void clearScreen();

void itoa(const int x, char* buf);
void itoa_s(const int x, char* buf, const size_t base, const char* base_chars);

char getch();
uint8 getScancode();

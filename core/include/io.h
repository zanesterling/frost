#pragma once

#define VID_MEM 0xB8000
#define COLS 80
#define LINES 25
#define CHAR_ATTRIB 15 /* white on black */

unsigned char _CurX, _CurY;

void Putch(const char c);
void Puts(const char* str);
void MoveCursor();

unsigned char inbyte(unsigned short port);
void outbyte(unsigned short port, unsigned char data);

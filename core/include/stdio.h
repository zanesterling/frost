#pragma once

#include <stdint.h>

#define VID_MEM 0xB8000
#define COLS 80
#define ROWS 25
#define VID_MEM_MAX (VID_MEM + COLS * ROWS * 2)
#define CHAR_ATTRIB 15 /* white on black */
#define TABWIDTH 4

#define BLACK         0x0
#define BLUE          0x1
#define GREEN         0x2
#define CYAN          0x3
#define RED           0x4
#define MAGENTA       0x5
#define BROWN         0x6
#define LIGHT_GREY    0x7
#define DARK_GREY     0x8
#define LIGHT_BLUE    0x9
#define LIGHT_GREEN   0xa
#define LIGHT_CYAN    0xb
#define LIGHT_RED     0xc
#define LIGHT_MAGENTA 0xd
#define YELLOW        0xe
#define WHITE         0xf

enum render_mode {
	RENDER_MODE_DIRECT,
	RENDER_MODE_BUFFERED,
};

struct cursor_loc {
	uint8_t x, y;
};

void init_rendering();

void putch(const char c);
void draw_char_at(
	char c,
	uint8_t fore,
	uint8_t back,
	int x,
	int y
);

void scroll(const uint8 numRows);
void puts(const char* str);
void printf(const char* fmt, ...);
void move_cursor(uint8_t x, uint8_t y);
struct cursor_loc get_cursor_loc();
void update_cursor();
void clear_screen();
void clear_rest_of_row();

int itoa(const int64_t x, char* buf);
int itoa_unsigned(const uint64_t x, char* buf);
int itoa_hex(uint64_t x, char* buf);
int itoa_s(
	const int64_t x,
	char* buf,
	const uint8_t base,
	const char* base_chars
);
int itoa_s_unsigned(
	const uint64_t x,
	char* buf,
	const uint8_t base,
	const char* base_chars
);

char getch();
int getch_nonblocking();
uint8 getScancode();

void set_render_mode(enum render_mode);
void display();

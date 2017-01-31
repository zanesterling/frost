#include "fireplace.h"


/* PRIVATE DEFINITONS */

#define VIEW_WIDTH  COLS
#define VIEW_HEIGHT ROWS

struct fireplace {
	uint8_t buf[VIEW_WIDTH * VIEW_HEIGHT];
};

struct fireplace _fireplace_new();
void _fireplace_update(struct fireplace*);
void _fireplace_render(struct fireplace*);

int16_t _get(struct fireplace* f, int x, int y);
void _set(struct fireplace* f, int x, int y, uint8_t val);

/* PUBLIC IMPLS */

void fireplace_run() {
	int c;
	struct fireplace f = _fireplace_new();

	do {
		_fireplace_update(&f);
		_fireplace_render(&f);
		c = getch();
	} while (1);//c < 0);

	clear_screen();
}


/* PRIVATE IMPLS */

void _add_clump(struct fireplace* f, int x, int y, int val) {
#define CLUMP_WIDTH 3
	for (int i = 0; i < CLUMP_WIDTH; i++) {
		for (int j = 0; j < CLUMP_WIDTH; j++) {
			_set(f, x + j, y + i, val);
		}
	}
}

struct fireplace _fireplace_new() {
	struct fireplace f;

	// zero the box
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			_set(&f, x, y, 0);
		}
	}

	// add coal at the bottom
	for (int x = 0; x < VIEW_WIDTH; x++) {
		_set(&f, x, 0, 255);
	}

	// add clumps
	_add_clump(&f, 3, 1, 255);
	_add_clump(&f, 10, 1, 255);
	_add_clump(&f, 23, 1, 255);

	return f;
}

void _fireplace_update(struct fireplace* f) {
	struct fireplace new_f = *f;

	// propagate up
	for (int x = 0; x < VIEW_WIDTH; x++) {
		_set(&new_f, x, 1, _get(f, x, 0) / 2);
	}

	for (int y = 2; y < VIEW_HEIGHT - 1; y++) {
		for (int x = 1; x < VIEW_WIDTH - 1; x++) {
			_set(&new_f, x, y, (
				_get(f, x - 1, y - 1) +
				_get(f, x + 1, y - 1) +
				_get(f, x    , y - 2) +
				_get(f, x    , y    )
			) / 2);
		}
	}

	memcpy(f->buf, new_f.buf, VIEW_WIDTH * VIEW_HEIGHT);
}

void _render_pixel(uint16_t val, int x, int y) {
	uint8_t COLORS[6] = {
		0x0, 0x4, 0x6, 0xc, 0xe, 0xf
	};
	draw_char_at(
		'x',
		COLORS[(val % 128) * 6 / 128],
		COLORS[(val - 128) * 6 / 128],
		x, ROWS - y - 1);
}

void _fireplace_render(struct fireplace* f) {
	for (int y = 0; y < VIEW_HEIGHT; y++) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			_render_pixel(_get(f, x, y), x, y);
		}
	}
}


int16_t _get(struct fireplace* f, int x, int y) {
	if (
		x < 0 || VIEW_WIDTH <= x ||
		y < 0 || VIEW_HEIGHT <= y
	) {
		return -1;
	}

	return f->buf[y * VIEW_WIDTH + x];
}

void _set(struct fireplace* f, int x, int y, uint8_t val) {
	if (
		x < 0 || VIEW_WIDTH <= x ||
		y < 0 || VIEW_HEIGHT <= y
	) {
		return;
	}

	f->buf[y * VIEW_WIDTH + x] = val;
}

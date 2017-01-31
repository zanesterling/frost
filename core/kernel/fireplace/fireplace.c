#include "fireplace.h"


/* PRIVATE DEFINITONS */

#define VIEW_WIDTH  80
#define VIEW_HEIGHT 25

struct fireplace {
	uint8_t buf[VIEW_WIDTH * VIEW_HEIGHT];
	int width, height;
};

struct fireplace _fireplace_new();
void _fireplace_update(struct fireplace*);
void _fireplace_render(struct fireplace*);

int16_t _get(struct fireplace* f, int x, int y);
void _set(struct fireplace* f, int x, int y, uint8_t val);

uint8_t COLORS[8] = { 0x0, 0x8, 0x6, 0x4, 0xc, 0xe, 0x9, 0xf };

/* PUBLIC IMPLS */

void fireplace_run() {
	int c;
	struct fireplace fireplace = _fireplace_new();

	do {
		_fireplace_update(&fireplace);
		_fireplace_render(&fireplace);
		c = getch_nonblocking();
	} while (c < 0);
	putch('\n');
	putch(c);
}


/* PRIVATE IMPLS */

struct fireplace _fireplace_new() {
	struct fireplace f;
	f.width = VIEW_WIDTH;
	f.height = VIEW_HEIGHT;

	// zero the box
	for (int i = 0; i < f.height; i++) {
		for (int j = 0; j < f.width; j++) {
			f.buf[i * f.width + j] = 0;
		}
	}

	// add coal at the bottom
	for (int x = 0; x < f.width; x++) {
		f.buf[x + (f.height - 1) * f.width] = 255;
	}

	return f;
}

void _fireplace_update(struct fireplace* f) {
	struct fireplace new_f = *f;

	// propagate up
	for (int x = 0; x < f->width; x++) {
		_set(&new_f, x, 1, _get(f, x, 0) - 1);
	}

	for (int y = 2; y < f->height - 1; y++) {
		for (int x = 1; x < f->width - 1; x++) {
			_set(&new_f, x, y, (
				_get(f, x - 1, y - 1) +
				_get(f, x + 1, y - 1) +
				_get(f, x    , y - 2) +
				_get(f, x    , y    )
			) / 4);
		}
	}

	*f = new_f;
}

void _render_pixel(uint8_t val, int x, int y) {
	draw_char_at('x', val / 32, val/ 32, x, y);
}

void _fireplace_render(struct fireplace* f) {
	for (int y = 0; y < f->height; y++) {
		for (int x = 0; x < f->width; x++) {
			_render_pixel(_get(f, x, y), x, y);
		}
	}
}


int16_t _get(struct fireplace* f, int x, int y) {
	if (x < 0 || f->width <= x || y < 0 || f->height <= y) {
		return -1;
	}

	return f->buf[y * f->width + x];
}

void _set(struct fireplace* f, int x, int y, uint8_t val) {
	if (x < 0 || f->width <= x || y < 0 || f->height <= y) {
		return;
	}

	f->buf[y * f->width + x] = val;
}

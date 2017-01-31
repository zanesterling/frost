#include "fireplace.h"


/* PRIVATE DEFINITONS */

#define VIEW_WIDTH  80
#define VIEW_HEIGHT 25

struct fireplace {
	uint8_t buf[VIEW_WIDTH * VIEW_HEIGHT];
	int width, height;
};

struct fireplace fireplace_new();
void fireplace_update(struct fireplace*);
void fireplace_render(struct fireplace*);


/* PUBLIC IMPLS */

void fireplace_run() {
	int c;
	struct fireplace fireplace = fireplace_new();

	do {
		fireplace_update(&fireplace);
		fireplace_render(&fireplace);
		c = getch_nonblocking();
	} while (c < 0);
	putch('\n');
	putch(c);
}


/* PRIVATE IMPLS */

struct fireplace fireplace_new() {
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

int16_t _get(struct fireplace* f, int x, int y) {
	if (x < 0 || f->width <= x || y < 0 || f->height <= y) {
		return -1;
	}

	return f->buf[y * f->width + x];
}

void fireplace_update(struct fireplace* f) {
	uint32_t num_pixels = f->width * f->height;
	uint8_t new_buf[f->width * f->height];

	// propagate up
	for (int y = 0; y < f->height; y++) {
		for (int x = 0; x < f->width; x++) {
			new_buf[y * f->width + x] = (
				_get(f, x - 1, y    ) +
				_get(f, x + 1, y    ) +
				_get(f, x    , y - 1) +
				_get(f, x    , y + 1)
			) / 4;
		}
	}

	memcpy(f->buf, new_buf, num_pixels);
}

void fireplace_render(struct fireplace* f) {
}

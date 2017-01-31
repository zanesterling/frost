#include "fireplace.h"


/* PRIVATE DEFINITONS */
struct fireplace {
	int x;
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
	f.x = 0;
	return f;
}

void fireplace_update(struct fireplace* f) {
}

void fireplace_render(struct fireplace* f) {
}

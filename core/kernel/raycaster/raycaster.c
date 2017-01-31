#include "raycaster.h"


/* PRIVATE DEFINITIONS */

#define VIEW_WIDTH  COLS
#define VIEW_HEIGHT ROWS

#define MAP_WIDTH  6
#define MAP_HEIGHT 4
const bool MAP[MAP_WIDTH * MAP_HEIGHT] = {
	false, true,  false, false, false, false,
	false, true,  true,  true,  true,  false,
	false, true,  false, false, true,  true,
	false, false, false, false, false, false,
};

typedef struct {
	int width, height;
	const bool* data;
} Map;

typedef struct {
	float x, y;
	Map map;

	bool w_pressed;
	bool a_pressed;
	bool s_pressed;
	bool d_pressed;
	bool q_pressed;
	bool e_pressed;
} RaycasterState;

RaycasterState new_RaycasterState();
Map new_Map();

void raycaster_update(RaycasterState*);
void raycaster_render(RaycasterState*);


/* PUBLIC IMPLS */

void raycaster_run() {
	RaycasterState state = new_RaycasterState();
	while (1) {
		raycaster_update(&state);
		raycaster_render(&state);

		int c;
		do {
			c = getch_nonblocking();
			switch (c) {
				case 'w':
					state.w_pressed = !state.w_pressed;
					break;
				case 'a':
					state.a_pressed = !state.a_pressed;
					break;
				case 's':
					state.s_pressed = !state.s_pressed;
					break;
				case 'd':
					state.d_pressed = !state.d_pressed;
					break;
				case 'q':
					state.q_pressed = !state.q_pressed;
					break;
				case 'e':
					state.e_pressed = !state.e_pressed;
					break;
			}
		} while (c != -1 && c != 'h');
		if (c == 'h') break;
	}
}


/* PRIVATE IMPLS */

RaycasterState new_RaycasterState() {
	RaycasterState state;

	state.x = 0;
	state.y = 0;

	state.w_pressed = false;
	state.a_pressed = false;
	state.s_pressed = false;
	state.d_pressed = false;
	state.q_pressed = false;
	state.e_pressed = false;

	state.map = new_Map();

	return state;
}

Map new_Map() {
	Map map;

	map.width = MAP_WIDTH;
	map.width = MAP_HEIGHT;
	map.data = MAP;

	return map;
}

void raycaster_update(RaycasterState* state) {}
void raycaster_render(RaycasterState* state) {}

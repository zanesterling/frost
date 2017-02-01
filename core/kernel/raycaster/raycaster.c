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

#define PLAYER_MOVE_SPEED 0.1
#define PLAYER_TURN_SPEED 0.1
typedef struct {
	float x, y, theta;
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
					state.w_pressed = true;
					break;
				case 'a':
					state.a_pressed = true;
					break;
				case 's':
					state.s_pressed = true;
					break;
				case 'd':
					state.d_pressed = true;
					break;
				case 'q':
					state.q_pressed = true;
					break;
				case 'e':
					state.e_pressed = true;
					break;
				case ' ':
					state.w_pressed = false;
					state.a_pressed = false;
					state.s_pressed = false;
					state.d_pressed = false;
					state.q_pressed = false;
					state.e_pressed = false;
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
	state.theta = M_TAU / 4;

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


void raycaster_update(RaycasterState* state) {
	if (state->w_pressed) {
		state->x += PLAYER_TURN_SPEED * cos(state->theta);
		state->y += PLAYER_TURN_SPEED * sin(state->theta);
	}
	if (state->a_pressed) {
		state->x += PLAYER_TURN_SPEED * sin(state->theta);
		state->y += PLAYER_TURN_SPEED * cos(state->theta);
	}
	if (state->s_pressed) {
		state->x -= PLAYER_TURN_SPEED * cos(state->theta);
		state->y -= PLAYER_TURN_SPEED * sin(state->theta);
	}
	if (state->d_pressed) {
		state->x -= PLAYER_TURN_SPEED * cos(state->theta);
		state->y -= PLAYER_TURN_SPEED * sin(state->theta);
	}

	if (state->q_pressed) state->theta += PLAYER_TURN_SPEED;
	if (state->e_pressed) state->theta -= PLAYER_TURN_SPEED;
}

void raycaster_render(RaycasterState* state) {
	uint8_t column_heights[VIEW_WIDTH];

	// Compute the heights of the columns
	for (int i = 0; i < VIEW_WIDTH; i++) column_heights[i] = 1;

	// Render the columns
	for (int column = 0; column < VIEW_WIDTH; column++) {
		uint8_t col_height = column_heights[column];
		for (int row = 0; row < VIEW_HEIGHT / 2 - col_height; row++) {
			draw_char_at(' ', 0, 0, column, row);
			draw_char_at(' ', 0, 0x8, column, (VIEW_HEIGHT - 1) - row);
		}

		for (
			int row = VIEW_HEIGHT / 2 - col_height;
			row <= VIEW_HEIGHT / 2;
			row++
		) {
			draw_char_at(' ', 0, 0xf, column, row);
			draw_char_at(' ', 0, 0xf, column, (VIEW_HEIGHT - 1) - row);
		}
	}
}

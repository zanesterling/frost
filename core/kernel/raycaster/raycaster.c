#include "raycaster.h"


/* PRIVATE DEFINITIONS */

#define VIEW_WIDTH  COLS
#define VIEW_HEIGHT ROWS

#define SCREEN_DIST  2f
#define SCREEN_WIDTH 1f

#define MAP_WIDTH  11
#define MAP_HEIGHT 9
const uint8_t MAP[MAP_WIDTH * MAP_HEIGHT] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

typedef struct {
	int width, height;
	const bool* data;
} Map;

#define PLAYER_MOVE_SPEED 0.001
#define PLAYER_TURN_SPEED 0.001
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

float _raycast(RaycasterState*, float angle);

bool _map_get(Map* map, int x, int y) {
	if (x < 0 || map->width < x || y < 0 || map->height < y) {
		return false;
	}
	return map->data[y * map->width + x];
}


/* PUBLIC IMPLS */

void raycaster_run() {
	//set_render_mode(RENDER_MODE_BUFFERED);

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

	//set_render_mode(RENDER_MODE_DIRECT);
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
		state->x -= PLAYER_TURN_SPEED * sin(state->theta);
		state->y += PLAYER_TURN_SPEED * cos(state->theta);
	}
	if (state->s_pressed) {
		state->x -= PLAYER_TURN_SPEED * cos(state->theta);
		state->y -= PLAYER_TURN_SPEED * sin(state->theta);
	}
	if (state->d_pressed) {
		state->x += PLAYER_TURN_SPEED * sin(state->theta);
		state->y -= PLAYER_TURN_SPEED * cos(state->theta);
	}

	if (state->q_pressed) state->theta += PLAYER_TURN_SPEED;
	if (state->e_pressed) state->theta -= PLAYER_TURN_SPEED;
}

void raycaster_render(RaycasterState* state) {
	float column_heights[VIEW_WIDTH];

	// Compute the heights of the columns
	for (int i = 0; i < VIEW_WIDTH; i++) {
		float render_plane_x = 2 * i / (float)VIEW_WIDTH - 1;
		float render_plane_angle = asin(render_plane_x);
		float ray_angle = state->theta + render_plane_angle;

		float distance = _raycast(state, ray_angle);

		if (distance < 0) {
			column_heights[VIEW_WIDTH] = 0;
		} else {
			column_heights[VIEW_WIDTH] = VIEW_HEIGHT / distance;
		}
	}

	// Render the columns
	for (int column = 0; column < VIEW_WIDTH; column++) {
		uint8_t col_height = column_heights[column];
		for (
			int row = 0;
			row <= VIEW_HEIGHT / 2 - col_height;
			row++
		) {
			if (row != VIEW_HEIGHT / 2) {
				draw_char_at(' ', 0, 0, column, row);
			}
			draw_char_at(
				' ', 0, 0x8,
				column, (VIEW_HEIGHT - 1) - row
			);
		}

		for (
			int row = VIEW_HEIGHT / 2 - col_height + 1;
			row <= VIEW_HEIGHT / 2;
			row++
		) {
			draw_char_at(' ', 0, 0xf, column, row);
			draw_char_at(
				' ', 0, 0xf,
				column, (VIEW_HEIGHT - 1) - row
			);
		}
	}

	// Debug rendering
	#define debug_render true
	if (debug_render) {
		move_cursor(0, 0);
		printf("state->x: %d\n", (int)state->x);
		printf("state->y: %d\n", (int)state->y);
	}

	//display();
}

float _raycast(RaycasterState* state, float ray_angle) {
	float ray_dir_x = cos(ray_angle);
	float ray_dir_y = sin(ray_angle);
	float ray_pos_x = state->x;
	float ray_pos_y = state->y;

	int map_x = ray_pos_x;
	int map_y = ray_pos_y;

	float delta_dist_x = sqrt(1 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x));
	float delta_dist_y = sqrt(1 + (ray_dir_x * ray_dir_x) / (ray_dir_y * ray_dir_y));

	float side_dist_x, side_dist_y;
	float step_x, step_y;
	if (ray_dir_x < 0) {
		step_x = -1;
		side_dist_x = (ray_pos_x - map_x) * delta_dist_x;
	} else {
		step_x = 1;
		side_dist_x = (map_x + 1 - ray_pos_x) * delta_dist_x;
	}
	if (ray_dir_y < 0) {
		step_y = -1;
		side_dist_y = (ray_pos_y - map_y) * delta_dist_y;
	} else {
		step_y = 1;
		side_dist_y = (map_y + 1 - ray_pos_y) * delta_dist_y;
	}

	int side;
	bool hit = false;
	while (!hit) {
		// Update our position.
		if (side_dist_x < side_dist_y) {
			side_dist_x += delta_dist_x;
			map_x += step_x;
			side = 0;
		} else {
			side_dist_y += delta_dist_y;
			map_y += step_y;
			side = 1;
		}

		// Check for out-of-bounds.
		if (
			0 < map_x || state->map.width  < map_x ||
			0 < map_y || state->map.height < map_y
		) {
			return -1;
		}

		// Check if ray has hit a wall.
		if (_map_get(&state->map, map_x, map_y) > 0) hit = true;
	}

	float distance;
	if (side == 0) {
		distance = (map_x - ray_pos_x + (1 - step_x) / 2)
			/ ray_dir_x;
	} else {
		distance = (map_y - ray_pos_y + (1 - step_y) / 2)
			/ ray_dir_y;
	}
	return distance;
}

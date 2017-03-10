#include "raycaster.h"


/* PRIVATE DEFINITIONS */

#define VIEW_WIDTH  COLS
#define VIEW_HEIGHT ROWS

#define PLANE_DIST  6.0f
#define PLANE_WIDTH 1.0f

#define MAP_WIDTH  12
#define MAP_HEIGHT 12
const uint8_t MAP[MAP_WIDTH * MAP_HEIGHT] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

typedef struct {
	int width, height;
	const bool* data;
} Map;

#define PLAYER_MOVE_SPEED 0.003
#define PLAYER_TURN_SPEED 0.002
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

struct Hit {
	float distance;
	int x, y;
	int side;
	float height;
};

RaycasterState new_RaycasterState();
Map new_Map();

void raycaster_update(RaycasterState*);
void raycaster_render(RaycasterState*);

struct Hit _raycast(RaycasterState*, float angle);

bool _map_get(Map* map, int x, int y) {
	if (x < 0 || map->width <= x || y < 0 || map->height <= y) {
		return false;
	}
	return map->data[y * map->width + x];
}


/* PUBLIC IMPLS */

void raycaster_run() {
	clear_screen();
	set_render_mode(RENDER_MODE_BUFFERED);
	move_cursor(0, 0);

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

		move_cursor(0,0);
		printf("x: %d, y: %d, theta: %d\n", (int)state.x, (int)state.y, (int)(1000 * state.theta));
		display();
	}

	set_render_mode(RENDER_MODE_DIRECT);
}


/* PRIVATE IMPLS */

RaycasterState new_RaycasterState() {
	RaycasterState state;

	state.x = 2;
	state.y = 2;
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

	map.width  = MAP_WIDTH;
	map.height = MAP_HEIGHT;
	map.data = MAP;

	return map;
}


void raycaster_update(RaycasterState* state) {
	if (state->w_pressed) {
		state->x += PLAYER_MOVE_SPEED * cos(state->theta);
		state->y -= PLAYER_MOVE_SPEED * sin(state->theta);
	}
	if (state->a_pressed) {
		state->x -= PLAYER_MOVE_SPEED * sin(state->theta);
		state->y -= PLAYER_MOVE_SPEED * cos(state->theta);
	}
	if (state->s_pressed) {
		state->x -= PLAYER_MOVE_SPEED * cos(state->theta);
		state->y += PLAYER_MOVE_SPEED * sin(state->theta);
	}
	if (state->d_pressed) {
		state->x += PLAYER_MOVE_SPEED * sin(state->theta);
		state->y += PLAYER_MOVE_SPEED * cos(state->theta);
	}

	if (state->q_pressed) {
		state->theta += PLAYER_TURN_SPEED;
		if (state->theta > M_TAU) state->theta -= M_TAU;
	}
	if (state->e_pressed) {
		state->theta -= PLAYER_TURN_SPEED;
		if (state->theta < 0) state->theta += M_TAU;
	}
}

void raycaster_render(RaycasterState* state) {
	struct Hit column_hits[VIEW_WIDTH];

	// Compute the heights of the columns
	for (int i = 0; i < VIEW_WIDTH; i++) {
		float camera_x = (i / (float)VIEW_WIDTH) * 2 - 1;
		float camera_angle = -asin(camera_x * PLANE_WIDTH / PLANE_DIST);
		float ray_angle = state->theta + camera_angle;

		struct Hit hit = _raycast(state, ray_angle);
		hit.height = (VIEW_HEIGHT / 2) / hit.distance;
		if (hit.height < 1) hit.height = 1;
		column_hits[i] = hit;
	}

	// Render the columns
	for (int column = 0; column < VIEW_WIDTH; column++) {
		struct Hit col_hit = column_hits[column];
		uint8_t col_height = col_hit.height;
		if (col_height > VIEW_HEIGHT / 2) col_height = VIEW_HEIGHT / 2;

		uint8_t col_start = (VIEW_HEIGHT / 2 + 1) - col_height;
		uint8_t col_end = (VIEW_HEIGHT / 2) + col_height;

		// sky and ground
		for (
			int row = 0;
			row < col_start && row <= col_end;
			row++
		) {
			if (row != VIEW_HEIGHT / 2) {
				draw_char_at(' ', 0, BLACK, column, row);
			}
			draw_char_at(
				' ', 0, DARK_GREY,
				column, (VIEW_HEIGHT - 1) - row
			);
		}

		uint8_t back_color = col_hit.side == 0 ? LIGHT_BLUE : LIGHT_GREEN;//WHITE;

		// columns
		for (
			int row = col_start;
			row < col_end;
			row++
		) {
			draw_char_at(' ', 0, back_color, column, row);
		}

		if (debug_render) {
			move_cursor(0, 2);
			printf("col_start: %d", (int)col_start);
			move_cursor(0, 3);
			printf("col_end:   %d", (int)col_end);
		}
	}

	// Debug rendering
	if (debug_render) {
		move_cursor(0, 0);
		printf("state->x: %d\n", (int)state->x);
		printf("state->y: %d\n", (int)state->y);
	}

	display();
}

struct Hit _raycast(RaycasterState* state, float ray_angle) {
	float ray_pos_x = state->x;
	float ray_pos_y = state->y;
	float ray_dir_x = cos(ray_angle);
	float ray_dir_y = -sin(ray_angle);

	int map_x = (int)ray_pos_x;
	int map_y = (int)ray_pos_y;

	float delta_dist_x = sqrt(1 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x));
	float delta_dist_y = sqrt(1 + (ray_dir_x * ray_dir_x) / (ray_dir_y * ray_dir_y));

	float side_dist_x, side_dist_y;
	int step_x, step_y;
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

	struct Hit hit_data;
	bool hit = false;
	while (!hit) {
		// Update our position.
		if (side_dist_x < side_dist_y) {
			side_dist_x += delta_dist_x;
			map_x += step_x;
			hit_data.side = 0;
		} else {
			side_dist_y += delta_dist_y;
			map_y += step_y;
			hit_data.side = 1;
		}

		// Check for out-of-bounds.
		if (
			map_x < 0 || state->map.width  <= map_x ||
			map_y < 0 || state->map.height <= map_y
		) {
			hit = true;
			break;
		}

		// Check if ray has hit a wall.
		hit = _map_get(&state->map, map_x, map_y);
	}

	hit_data.x = map_x;
	hit_data.y = map_y;
	if (hit_data.side == 0) {
		if (step_x < 0) hit_data.x++;
		hit_data.distance = (hit_data.x - ray_pos_x) / ray_dir_x;
	} else {
		if (step_y < 0) hit_data.y++;
		hit_data.distance = (hit_data.y - ray_pos_y) / ray_dir_y;
	}
	return hit_data;
}

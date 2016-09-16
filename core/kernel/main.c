#include "main.h"

int main() {
	puts("Hello, world of kernels\n\n");
	int init_err = init();
	if (init_err) printf("init_err: %x\n", init_err);

	// fetch and run commands
	char cmd_buf[128];
	while (1) {
		get_cmd(cmd_buf, 126);
		puts(cmd_buf);
		putch('\n');
	}

	puts("\nThanks for using FrOSt\n");
	for (;;);
	return 0;
}

int init() {
	int hal_err = hal_initialize();
	if (hal_err) return hal_err;

	install_handlers();
	enable_interrupts();
	kbrd_install(33);

	return 0;
}

void sleep(uint32_t ms) {
	uint32_t ticks = ms + get_tick_count();
	while (ticks > get_tick_count());
}

void get_cmd(char* cmd_buf, size_t buflen) {
	puts("? ");

	size_t i = 0;
	KEYCODE key = KEY_UNKNOWN;
	while (i < buflen) {
		key = getch();

		if (key == KEY_RETURN) {
			putch('\n');
			break;
		} else if (key == KEY_BACKSPACE) {
			// if the buffer isn't empty
			if (i > 0) {
				// delete last char
				struct cursor_loc pos = get_cursor_loc();
				if (pos.x > 0) {
					move_cursor(--pos.x, pos.y);
				} else {
					pos.x = COLS - 1;
					move_cursor(pos.x, --pos.y);
				}

				putch(' ');
				move_cursor(pos.x, pos.y);
				i--;
				update_cursor();
			}
		} else {
			char c = kbrd_key_to_ascii(key);
			if (c != 0) {
				putch(c);
				cmd_buf[i++] = c;
			}
		}
	}

	cmd_buf[i] = '\0';
}

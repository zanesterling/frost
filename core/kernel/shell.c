#include "shell.h"

bool _running;

// PRIVATE FUNCTION DECLARATIONS

void get_cmd(char* cmd_buf, size_t buflen);
void run_cmd(char* cmd);


// PUBLIC FUNCTION IMPLEMENTATIONS

void run_shell() {
	// fetch and run commands
	char cmd_buf[128];
	_running = true;
	while (_running) {
		get_cmd(cmd_buf, 126);
		run_cmd(cmd_buf);
	}
}


// PRIVATE FUNCTION IMPLEMENTATIONS

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

void run_cmd(char* cmd) {
	if (strcmp(cmd, "exit") == 0) {
		_running = false;
	} else if (strcmp(cmd, "help") == 0) {
		puts(
			"help: lists runnable commands\n"
			"exit: exits FrOSt\n"
		);
	} else {
		printf("command %s not recognized\n", cmd);
	}
}

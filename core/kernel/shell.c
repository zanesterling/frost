#include "shell.h"

bool _running;

// PRIVATE FUNCTION DECLARATIONS

void get_cmd(char* cmd_buf, size_t buflen);
void run_cmd(char* cmd);
void help();
void mmap_summary();


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
	clear_rest_of_row();

	size_t i = 0;
	KEYCODE key = KEY_UNKNOWN;
	while (i < buflen - 1) {
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
	if (*cmd == '\0') return;

	if (strcmp(cmd, "help") == 0) {
		help();
	} else if (strcmp(cmd, "clear") == 0) {
		clear_screen();
	} else if (strcmp(cmd, "mmap") == 0) {
		mmap_summary();
	} else if (strcmp(cmd, "pmem") == 0) {
		pmem_print_summary();
	} else if (strcmp(cmd, "sleep") == 0) {
		puts("sleeping...");
		sleep(100);
		puts("done\n");
	} else if (strcmp(cmd, "fireplace") == 0) {
		fireplace_run();
	} else if (strcmp(cmd, "raycaster") == 0) {
		raycaster_run();
	} else if (strcmp(cmd, "exit") == 0) {
		_running = false;
	} else {
		printf("command '%s' not recognized\n", cmd);
	}
}

void help() {
	puts(
		"help: list runnable commands\n"
		"clear: blank the display\n"
		"mmap: show the BIOS memory map\n"
		"pmem: show a summary of the physical memory allocation\n"
		"sleep: sleep for a few seconds\n"
		"fireplace: a color display demo\n"
		"raycaster: a display and input demo\n"
		"exit: exit FrOSt\n"
	);
}

void mmap_summary() {
	multiboot_info* bootinfo = get_bootinfo();

	uint64_t memory_size = 1024 + bootinfo->memoryLo
		+ 64 * bootinfo->memoryHi;
	printf("Memory size: %d KB (0x%x KB)\n", memory_size, memory_size);

	printf("Memory map: (%d elements)\n", bootinfo->mmap_length);
	for (uint32_t i = 0; i < bootinfo->mmap_length; i++) {
		mmap_entry entry = ((mmap_entry*) bootinfo->mmap_addr)[i];

		printf(
			"  %#012llx - %#012llx: %s\n",
			entry.base_address,
			entry.base_address + entry.length,
			TYPE_STRING(entry.type)
		);
	}
}

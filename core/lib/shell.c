#include "shell.h"

/* Used to dodge memory allocation.
 * TODO: implement memory allocation.
 */
#define BUF_SIZE 1024
char buf[BUF_SIZE];

void runShell(ShellState* state) {
	putch('\n');
	while (!state->shouldExit) {
		char* line = prompt("> ");
		puts(line);
	}
}

char* prompt(char* promptStr) {
	puts(promptStr);

	for (size_t i = 0; i < BUF_SIZE; i++) {
		buf[i] = getch();
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
		putch(buf[i]);
	}
	return buf;
}

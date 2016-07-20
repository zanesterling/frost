#include "main.h"

int main() {
	puts("Hello, world of kernels\n");

	ShellState state;
	runShell(&state);
	return 0;
}

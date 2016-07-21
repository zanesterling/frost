#include "main.h"

int main() {
	hal_initialize();

	puts("Hello, world of kernels\n");

	//ShellState state;
	//runShell(&state);

	hal_shutdown();
	return 0;
}

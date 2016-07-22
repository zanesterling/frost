#include "main.h"

int main() {
	puts("Hello, world of kernels\n");
	int hal_err = hal_initialize();
	if (hal_err) printf("hal_err: %x\n", hal_err);

	geninterrupt(0);

	//ShellState state;
	//runShell(&state);

	hal_shutdown();
	return 0;
}

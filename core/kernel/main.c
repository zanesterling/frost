#include "main.h"

int main() {
	puts("Hello, world of kernels\n\n");
	int init_err = init();
	if (init_err) {
		printf("init_err: %x\n", init_err);
		for (;;);
	}

	run_shell();

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


#include "main.h"

int kernel_main(multiboot_info* info) {
	puts("Hello, world of kernels\n\n");
	int init_err = init(info);
	if (init_err) {
		printf("init_err: %x\n", init_err);
	} else {
		run_shell();
		puts("\nThanks for using FrOSt\n");
	}

	asm(
		"cli\n"
		"hlt\n"
	);
	return 0;
}

int init(multiboot_info* info) {
	int hal_err = hal_initialize(info);
	if (hal_err) return hal_err;

	install_handlers();
	enable_interrupts();
	kbrd_install(33);

	return 0;
}

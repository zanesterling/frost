#include "main.h"

int kernel_main(multiboot_info* info) {
	puts("Hello, world of kernels\n\n");
	int init_err = init();
	if (init_err) {
		printf("init_err: %x\n", init_err);
		for (;;);
	}

	memory_summary(info);
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

void memory_summary(multiboot_info* bootinfo) {
	printf("bootinfo*: 0x%x\n", bootinfo);
	printf("hi:0x%x\nlo:0x%x\n", bootinfo->memoryHi, bootinfo->memoryLo);
	uint64_t memory_size = 1024 + bootinfo->memoryLo + bootinfo->memoryHi * 64;
	printf("Memory size: 0x%d KB\n", memory_size);
}

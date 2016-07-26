#include "panic.h"

void kernel_panic(char* err_msg) {
	// disable all interrupts
	disable();

	puts(err_msg);
	puts("\nEVERYBODY PANIC\n");
	for (;;);
}

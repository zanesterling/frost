#include "panic.h"

void panic(char* err_msg) {
	// disable all interrupts
	disable_interrupts();

	puts(err_msg);
	puts("\nEVERYBODY PANIC\n");
	for (;;);
}

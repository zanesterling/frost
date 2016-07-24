#include "main.h"

void int_handler_5();

int main() {
	puts("Hello, world of kernels\n\n");
	int hal_err = hal_initialize();
	if (hal_err) printf("hal_err: %x\n", hal_err);

	i86_install_ir(5, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, 8, (I86_IRQ_HANDLER)int_handler_5);
	puts("generating interrupt 5\n");
	geninterrupt(5);
	puts("done interrupting\n");

	//ShellState state;
	//runShell(&state);

	hal_shutdown();
	return 0;
}

void int_handler_5() {
	asm(
		"pushad\n"
		::: "memory", "esp"
	);

	puts("caught interrupt 5!\n");

	asm(
		"popad\n"
		"leave\n"
		"iretd\n"
		::: "memory", "esp"
	);
}

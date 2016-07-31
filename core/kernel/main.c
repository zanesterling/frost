#include "main.h"

void int_handler_5();

int main() {
	puts("Hello, world of kernels\n\n");

	int hal_err = hal_initialize();
	if (hal_err) printf("hal_err: %x\n", hal_err);

	// Install exception handlers
	install_handlers();

	// Enable interrupts
	enable();

	setvect(5, int_handler_5);
	puts("generating interrupt 5\n");
	geninterrupt(5);
	puts("done interrupting\n");

	struct cursor_loc cursor = get_cursor_loc();
	move_cursor(0, 14);
	puts("Current tick count: ");
	move_cursor(cursor.x, cursor.y);
	for (;;) {
		cursor = get_cursor_loc();
		move_cursor(20, 14);
		printf("%d\n", get_tick_count());
		move_cursor(cursor.x, cursor.y);
	}

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

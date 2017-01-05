#include "scheduler.h"

/* Private data */

static bool _thread_sleeping = false;
struct SleepingThread _sleeper;


/* Private function defs */


/* Public function impls */

void init_scheduler() {
	_thread_sleeping = false;
}

void scheduler_tick_handler() {
	asm(
		"pushad\n"
		::: "memory", "esp"
	);

	if (_thread_sleeping) {
		if (_sleeper.ticks_to_sleep > 0) {
			_sleeper.ticks_to_sleep--;
		} else {
			asm(
				"mov dword [esp + 4*10], %0\n"
				:: "r" (_sleeper.ret_addr)
			);
			_thread_sleeping = false;
		}
	}

	interruptdone(0);
	asm(
		"popad\n"
		"leave\n"
		"iretd\n"
		::: "memory", "esp"
	);
}

void sleep(uint32_t ticks) {
	asm(
		"pushad\n"
		"cli\n"
	);

	if (_thread_sleeping) {
		puts("error: tried to sleep while a thread was sleeping\n");
		asm(
			"cli\n"
			"hlt\n"
		);
	}
	_thread_sleeping = true;

	// store return address of sleeper
	_sleeper.ret_addr = &&after;
	_sleeper.ticks_to_sleep = ticks;
	asm(
		"sti\n"
		"loop: hlt\n"
		"jmp loop\n"
	);

	// Removing this nop fakes out the puts("done") in the shell.
	after: asm("nop\n");
	return;
}


/* Private function impls */

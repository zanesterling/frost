#pragma once

#include <hal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void init_scheduler();
void scheduler_tick_handler();
void sleep(uint32_t ticks);

struct SleepingThread {
	void* ret_addr;
	uint32_t ticks_to_sleep;
};

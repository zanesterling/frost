#pragma once

#include <pmem.h>
#include <stdint.h>

#include "spinlock.h"

struct process {
	uint32_t pid;
	bool in_use;
};

int init_proclist();
struct process* new_process();
struct process* get_process(uint32_t pid);

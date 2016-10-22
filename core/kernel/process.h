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
void free_process(struct process* proc);
struct process* get_process(uint32_t pid);

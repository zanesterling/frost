#pragma once

#include <pmem.h>
#include <stdint.h>

struct process {
	uint32_t pid;
};

int init_proclist();
struct process* get_process(uint32_t pid);

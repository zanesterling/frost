#include "bootinfo.h"

uint64_t get_memory_size_kb(struct multiboot_info* bootinfo) {
	return 1024 + bootinfo->memoryLo + 64 * bootinfo->memoryHi;
}

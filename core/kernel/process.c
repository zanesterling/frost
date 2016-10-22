#include "process.h"

#define MAX_PROCS 256
struct process* _proclist;

#define _is_valid_pid(pid) ((pid) > 0 && (pid) < MAX_PROCS)

// PUBLIC FUNC IMPLS

int init_proclist() {
	size_t mem_needed = MAX_PROCS * sizeof(struct process);
	_proclist = (struct process*) pmem_alloc_memory(mem_needed);
	if (_proclist == NULL) return -1;
	return 0;
}

struct process* get_process(uint32_t pid) {
	if (!_is_valid_pid(pid)) return NULL;
	return _proclist + pid;
}

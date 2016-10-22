#include "process.h"

#define MAX_PROCS 256
SpinLock _proclist_lock;
struct process* _proclist;
int _num_procs;

#define _is_valid_pid(pid) ((pid) > 0 && (pid) < MAX_PROCS)

// PUBLIC FUNC IMPLS

int init_proclist() {
	size_t mem_needed = MAX_PROCS * sizeof(struct process);
	_proclist = (struct process*) pmem_alloc_memory(mem_needed);
	if (_proclist == NULL) return -1;

	for (int i = 0; i < MAX_PROCS; i++) {
		_proclist[i].pid = i;
		_proclist[i].in_use = false;
	}

	_num_procs = 0;
	_proclist_lock.locked = 0;
	return 0;
}

struct process* new_process() {
	lock(&_proclist_lock);
	if (_num_procs >= MAX_PROCS) {
		unlock(&_proclist_lock);
		return NULL;
	}

	for (int i = 0; i < MAX_PROCS; i++) {
		if (_proclist[i].in_use) continue;

		_num_procs++;
		_proclist[i].in_use = false;
		unlock(&_proclist_lock);
		return _proclist + i;
	}

	unlock(&_proclist_lock);
	return NULL;
}

void free_process(struct process* proc) {
	proc->in_use = false;
}

struct process* get_process(uint32_t pid) {
	if (!_is_valid_pid(pid)) return NULL;
	return _proclist + pid;
}

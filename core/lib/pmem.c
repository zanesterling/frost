#include <pmem.h>

// LOCAL VARIABLES
uint64_t _mem_size = 0; // in KB
uint8_t* _memory_map = 0;
int64_t _max_blocks = 0;
int64_t _used_blocks = 0;

#define BLOCK_SIZE_BITS 12


// PRIVATE FUNCTION DEFS
#define _set_bit(block)   _memory_map[(block) / 8] |=   1 << ((block) % 8)
#define _unset_bit(block) _memory_map[(block) / 8] &= ~(1 << ((block) % 8))
#define _test_bit(block) (_memory_map[(block) / 8] &  (1 << ((block) % 8)))

void _set_block(void* p, uint8_t on);
int64_t _get_first_free_block();
int64_t _get_first_free_blocks(uint32_t num_blocks);


// PUBLIC FUNCTION IMPLS
void pmem_init(size_t mem_size, uint32_t* bitmap, struct mem_map memory_map) {
	_mem_size = mem_size;
	_memory_map = (uint8_t*) bitmap;
	_max_blocks = _mem_size / PMEM_BLOCK_SIZE * 1024;
	_used_blocks = _max_blocks;

	// all memory is in use by default
	memset(_memory_map, 0xf, _max_blocks * PMEM_BLOCK_SIZE);
	_used_blocks = _max_blocks;

	// parse memory map and unset available memory
	struct mmap_entry* entry = memory_map.addr;
	for (uint32_t i = 0; i < memory_map.len; i++) {
		if (
			entry->type == MMAP_TYPE_AVAILABLE
			&& entry->length > PMEM_BLOCK_SIZE
		) {
			// align to block boundaries
			uint64_t start_block = (entry->base_address + PMEM_BLOCK_SIZE - 1)
				/ PMEM_BLOCK_SIZE;
			void* start_pointer = (void*)(uint32_t) (start_block * PMEM_BLOCK_SIZE);

			// account for alignment
			uint32_t shift = PMEM_BLOCK_SIZE
				- (entry->base_address % PMEM_BLOCK_SIZE);
			uint32_t num_blocks = (entry->length - shift) / PMEM_BLOCK_SIZE;

			// unset blocks
			pmem_free_blocks(start_pointer, num_blocks);
		}

		entry++;
	}
}

void pmem_init_region(void* base, int64_t size) {
	uint8_t* p = base;
	while (size--) {
		_set_block(p, 1);
		p += PMEM_BLOCK_SIZE;
	}
}

void pmem_deinit_region(void* base, int64_t size) {
	uint8_t* p = base;
	while (size--) {
		_set_block(p, 0);
		p += PMEM_BLOCK_SIZE;
	}
}

void* pmem_alloc_block() {
	int64_t block = _get_first_free_block();
	if (block < 0) return NULL;

	_used_blocks++;
	return (void*) ((uint32_t)block * PMEM_BLOCK_SIZE);
}

void* pmem_alloc_blocks(uint32_t num_blocks) {
	int64_t start = _get_first_free_blocks(num_blocks);
	if (start < 0) return NULL;

	_used_blocks += num_blocks;
	return (void*) ((uint32_t)start * PMEM_BLOCK_SIZE);
}

void pmem_free_block(void* block) {
	_set_block(block, 0);
	_used_blocks--;
}

void pmem_free_blocks(void* block, uint32_t num_blocks) {
	int64_t cur_block = ((int64_t) (uint32_t) block) >> BLOCK_SIZE_BITS;
	_used_blocks -= num_blocks;
	while (num_blocks--) {
		_unset_bit(cur_block);
		cur_block += 1;
	}
}

void pmem_print_summary() {
    for (uint32_t block = 0; block < _max_blocks; block++) {
    }
}


// PRIVATE FUNCTION IMPLS
// note: on MUST be 1 or 0
void _set_block(void* p, uint8_t on) {
	int64_t block = ((int64_t) (uint32_t) p) >> BLOCK_SIZE_BITS;
	if (on) _set_bit(block);
	else    _unset_bit(block);
}

int64_t _get_first_free_block() {
	if (_used_blocks == _max_blocks) return -1;

	for (int64_t i = 0; i < _max_blocks; i += 8) {
		// check for full byte
		if (_memory_map[i / 8] == 0xff) continue;

		for (int j = 0; j < 8; j++) {
			if (!_test_bit(i + j)) {
				return i + j;
			}
		}
	}

	return -1;
}

int64_t _get_first_free_blocks(uint32_t num_blocks) {
	if (_max_blocks - _used_blocks < num_blocks) return -1;

	for (int64_t i = 0; i < _max_blocks; i++) {
		int found = 1;
		for (uint32_t j = 0; j < num_blocks; j++) {
			if (_test_bit(i + j)) {
				found = 0;
				break;
			}
		}

		if (found) return i;
	}

	return -1;
}

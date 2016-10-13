#include <pmem.h>

size_t _mem_size = 0; // in KB
uint8_t* _memory_map = 0;
size_t _max_blocks = 0;
size_t _used_blocks = 0;

#define BLOCK_SIZE_BITS 12

#define _set_bit(block)    _memory_map[(block) / 8] |=   1 << ((block) % 8)
#define _unset_bit(block)  _memory_map[(block) / 8] &= ~(1 << ((block) % 8))
#define _test_bit(block)  (_memory_map[(block) / 8] &   (1 << ((block) % 8)))

void _set_block(void* p, uint8_t on);


void pmem_init(size_t mem_size, uint32_t* bitmap) {
	_mem_size = mem_size;
	_memory_map = (uint8_t*) bitmap;
	_max_blocks = _mem_size * (1024 / PMEM_BLOCK_SIZE);
	_used_blocks = _max_blocks;

	// all memory is in use by default
	memset(_memory_map, 0xf, _max_blocks * PMEM_BLOCK_SIZE);
}

void pmem_init_region(void* base, size_t size) {
	uint8_t* p = base;
	while (size--) {
		_set_block(p, 1);
		p += PMEM_BLOCK_SIZE;
	}
}

void pmem_deinit_region(void* base, size_t size) {
	uint8_t* p = base;
	while (size--) {
		_set_block(p, 0);
		p += PMEM_BLOCK_SIZE;
	}
}

void* pmem_alloc_block() {
	if (_used_blocks == _max_blocks) return NULL;

	uint8_t* p = _memory_map;
	for (size_t i = 0; i < _max_blocks; i += 8) {
		// check for full byte
		if (*p++ == 0xff) continue;

		for (int j = 0; j < 8; j++) {
			if (!_test_bit(i + j)) {
				_set_bit(i + j);
				_used_blocks++;
				return (void*) ((i + j) * PMEM_BLOCK_SIZE);
			}
		}
	}

	return NULL;
}

void pmem_free_block(void* block) {
	_set_block(block, 0);
	_used_blocks--;
}


// note: on MUST be 1 or 0
void _set_block(void* p, uint8_t on) {
	size_t block = ((size_t) p) >> BLOCK_SIZE_BITS;
	if (on) _set_bit(block);
	else    _unset_bit(block);
}

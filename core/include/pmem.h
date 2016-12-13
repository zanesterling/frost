#pragma once

#include <bootinfo.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PMEM_BLOCK_SIZE 4096

typedef uint32_t physical_addr;

// TODO: Make this multiprocess safe

void pmem_init(size_t memSize, uint32_t* bitmap, struct mem_map memory_map);
void pmem_init_region(void* base, int64_t size);
void pmem_deinit_region(void* base, int64_t size);

void* pmem_alloc_block();
void* pmem_alloc_blocks(uint32_t num_blocks);
void* pmem_alloc_memory(size_t mem_size);
void pmem_free_block(void* block);
void pmem_free_blocks(void* block, uint32_t num_blocks);

void pmem_print_summary();

#pragma once

#include <stdint.h>

typedef struct multiboot_info {
	uint32_t flags;
	uint32_t memoryLo;
	uint32_t memoryHi;
	uint32_t bootDevice;
	uint32_t cmdLine;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t syms0;
	uint32_t syms1;
	uint32_t syms2;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t bootloader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint16_t vbe_mode_info;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
} multiboot_info;

typedef struct mmap_entry {
	uint64_t base_address;
	uint64_t length;
	uint32_t type;
	uint32_t acpi_null; // reserved
} mmap_entry;

struct mem_map {
	uint32_t len;
	struct mmap_entry* addr;
};

enum MMAP_ENTRY_TYPE {
	MMAP_TYPE_AVAILABLE    = 1,
	MMAP_TYPE_RESERVED     = 2,
	MMAP_TYPE_ACPI_RECLAIM = 3,
	MMAP_TYPE_ACPI_NVS     = 4,
};

#define TYPE_STRING(type) ( \
	type == MMAP_TYPE_AVAILABLE    ? "available"    : \
	(type == MMAP_TYPE_RESERVED     ? "reserved"     : \
	(type == MMAP_TYPE_ACPI_RECLAIM ? "acpi reclaim" : \
	(type == MMAP_TYPE_ACPI_NVS     ? "acpi nvs"     : "bad type"))))

uint64_t get_memory_size_kb(struct multiboot_info* bootinfo);

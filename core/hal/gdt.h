#pragma once

#include "stdint.h"
#include "string.h"

#define MAX_DESCRIPTORS 16

enum GDT_DESC_MASK {
	GDT_DESC_MASK_ACCESS         = 0x0001,
	GDT_DESC_MASK_READWRITE      = 0x0002,
	GDT_DESC_MASK_EXPANSION_DIR  = 0x0004, // data segments
	GDT_DESC_MASK_CONFORMING     = 0x0004, // code segments
	GDT_DESC_MASK_EXECUTABLE     = 0x0008,
	GDT_DESC_MASK_IS_CODEORDATA  = 0x0010,
	GDT_DESC_MASK_PRIVILEGE      = 0x0060,
	GDT_DESC_MASK_SEG_IN_MEM     = 0x0080,
	GDT_DESC_MASK_SEG_LIMIT_HIGH = 0x0f00,
	GDT_DESC_MASK_RESERVED_OS    = 0x1000,
	GDT_DESC_MASK_RESERVED_ZERO  = 0x2000,
	GDT_DESC_MASK_IS_32BIT       = 0x4000,
	GDT_DESC_MASK_GRANULARITY    = 0x8000,
};

#pragma pack(push, 1)

struct gdt_descriptor {
	uint16_t limit;

	uint16_t baseLow;
	uint8_t baseMid;

	uint16_t flags;
	uint8_t baseHigh;
} gdt_descriptor;

#pragma pack(pop)

void gdt_set_descriptor(
	uint32_t desc_num, uint32_t base, uint32_t limit, uint16_t access
);

int i86_gdt_initialize();

void i86_default_handler();

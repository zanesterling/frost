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
	GDT_DESC_MASK_DESCRIPTOR     = 0x0010,
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

struct gdtr {
	uint16_t m_limit;
	uint32_t m_base;
} gdtr;
#pragma pack(pop)

static struct gdt_descriptor _gdt[MAX_DESCRIPTORS];
static struct gdtr _gdtr;

static void gdt_install() {
	asm("lgdt (%0)" :: "r" (&_gdtr));
}

void gdt_set_descriptor(
	uint32_t desc_num, uint32_t base, uint32_t limit, uint16_t access
) {
	if (desc_num >= MAX_DESCRIPTORS) return;
	memset(_gdt + desc_num, 0, sizeof(gdt_descriptor));

	_gdt[desc_num].baseLow  = base & 0xffff;
	_gdt[desc_num].baseMid  = (base >> 16) & 0xff;
	_gdt[desc_num].baseHigh = (base >> 24) & 0xff;
	_gdt[desc_num].limit    = limit & 0xffff;

	_gdt[desc_num].flags = access & 0xf0ff;
	_gdt[desc_num].flags |= (limit >> 8) & 0x0f00;
}

int i86_gdt_intiialize() {
	_gdtr.m_limit = (sizeof(gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	_gdtr.m_base = (uint32_t)_gdt;

	// null descriptor
	gdt_set_descriptor(0, 0, 0, 0);

	// default code descriptor
	gdt_set_descriptor(
		1, 0, 0xffff,
		GDT_DESC_MASK_READWRITE | GDT_DESC_MASK_EXECUTABLE |
		GDT_DESC_MASK_EXECUTABLE | GDT_DESC_MASK_SEG_IN_MEM |
		GDT_DESC_MASK_GRANULARITY | GDT_DESC_MASK_IS_32BIT | GDT_DESC_MASK_SEG_LIMIT_HIGH
	);

	// default data descriptor
	gdt_set_descriptor(
		2, 0, 0xffff,
		GDT_DESC_MASK_READWRITE | GDT_DESC_MASK_DESCRIPTOR | GDT_DESC_MASK_SEG_IN_MEM |
		GDT_DESC_MASK_GRANULARITY | GDT_DESC_MASK_IS_32BIT | GDT_DESC_MASK_SEG_LIMIT_HIGH
	);

	gdt_install();
	return 0;
}

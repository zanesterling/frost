#include "gdt.h"

/* PRIVATE STUFF */
#pragma pack(push, 1)

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

/* PUBLIC STUFF */

void gdt_set_descriptor(
	uint32_t desc_num, uint32_t base, uint32_t limit, uint16_t access
) {
	if (desc_num >= MAX_DESCRIPTORS) return;
	memset(_gdt + desc_num, 0, sizeof(gdt_descriptor));

	_gdt[desc_num].limit    = limit & 0xffff;
	_gdt[desc_num].baseLow  = base & 0xffff;
	_gdt[desc_num].baseMid  = (base >> 16) & 0xff;

	_gdt[desc_num].flags  = access & 0xf0ff;
	_gdt[desc_num].flags |= (limit & 0x0f0000) >> 8;

	_gdt[desc_num].baseHigh = (base >> 24) & 0xff;
}

int i86_gdt_initialize() {
	_gdtr.m_limit = (sizeof(gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	_gdtr.m_base = (uint32_t)_gdt;

	// null descriptor
	gdt_set_descriptor(0, 0, 0, 0);

	// default code descriptor
	gdt_set_descriptor(
		1, 0, 0xfffff,
		GDT_DESC_MASK_READWRITE | GDT_DESC_MASK_EXECUTABLE |
		GDT_DESC_MASK_IS_CODEORDATA | GDT_DESC_MASK_SEG_IN_MEM |
		GDT_DESC_MASK_GRANULARITY | GDT_DESC_MASK_IS_32BIT
	);

	// default data descriptor
	gdt_set_descriptor(
		2, 0, 0xfffff,
		GDT_DESC_MASK_READWRITE | GDT_DESC_MASK_IS_CODEORDATA | GDT_DESC_MASK_SEG_IN_MEM |
		GDT_DESC_MASK_GRANULARITY | GDT_DESC_MASK_IS_32BIT
	);

	gdt_install();
	return 0;
}

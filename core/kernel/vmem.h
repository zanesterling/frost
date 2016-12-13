#pragma once

#include <bool.h>
#include <stdint.h>
#include <pmem.h>

#define PTE_PRESENT        (1u << 0)
#define PTE_READ_WRITE     (1u << 1)
#define PTE_USER_SUPER     (1u << 2)
#define PTE_RESERVED1      (1u << 3)
#define PTE_RESERVED2      (1u << 4)
#define PTE_BEEN_ACCESSED  (1u << 5)
#define PTE_IS_DIRTY       (1u << 6)
#define PTE_RESERVED3      (1u << 7)
#define PTE_RESERVED4      (1u << 8)
#define PTE_IS_AVAILABLE   0x00000e00
#define PTE_FRAME_BASE     0xfffff000

#define PDE_PRESENT        (1u << 0)
#define PDE_READ_WRITE     (1u << 1)
#define PDE_USER_SUPER     (1u << 2)
#define PDE_WRITE_THRU     (1u << 3)
#define PDE_CACHE_DISABLED (1u << 4)
#define PDE_BEEN_ACCESSED  (1u << 5)
#define PDE_RESERVED       (1u << 6)
#define PDE_PAGE_SIZE      (1u << 7)
#define PDE_IS_GLOBAL      (1u << 8)
#define PDE_IS_AVAILABLE   0x00000e00
#define PDE_FRAME_BASE     0xfffff000

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIR  1024

typedef uint32_t pt_entry;
typedef uint32_t pd_entry;

typedef uint32_t virtual_addr;

struct ptable {
	pt_entry entries[PAGES_PER_TABLE];
};

struct pdirectory {
	pd_entry entries[TABLES_PER_DIR];
};

// function defs
void pte_set_attrib(pt_entry* entry, uint32_t attrib);
void pte_unset_attrib(pt_entry* entry, uint32_t attrib);
void pde_set_attrib(pd_entry* entry, uint32_t attrib);
void pde_unset_attrib(pd_entry* entry, uint32_t attrib);

bool pte_is_present(pt_entry entry);
bool pte_is_available(pt_entry entry);
physical_addr pte_frame_base(pt_entry entry);
void pte_set_frame(pt_entry*, physical_addr);

bool pde_is_present(pd_entry entry);
bool pde_is_available(pd_entry entry);
physical_addr pde_frame_base(pd_entry entry);
void pde_set_frame(pt_entry*, physical_addr);

bool vmem_alloc_page(pt_entry* e);
void vmem_free_page(pt_entry* e);
pt_entry* vmem_pte_lookup(struct ptable* table, virtual_addr addr);
pd_entry* vmem_pde_lookup(struct pdirectory* dir, virtual_addr addr);

void vmem_init();

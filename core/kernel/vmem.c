#include "vmem.h"

// LOCAL DEFINITIONS
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)     (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*(x) & 0xfffff000)

#define PTABLE_ADDR_SPACE_SIZE 0x400000
#define DTABLE_ADDR_SPACE_SIZE 0x100000000

#define PAGE_SIZE 4096

// GLOBAL FUNCTION DEFS
void pte_set_attrib  (pt_entry* e, uint32_t attr) { *e |= attr;  }
void pte_unset_attrib(pt_entry* e, uint32_t attr) { *e &= ~attr; }
void pde_set_attrib  (pd_entry* e, uint32_t attr) { *e |= attr;  }
void pde_unset_attrib(pd_entry* e, uint32_t attr) { *e &= ~attr; }

bool pte_is_present(pt_entry entry) { return entry & PTE_PRESENT; }
bool pte_is_available(pt_entry entry) {
	return (entry & PTE_IS_AVAILABLE) >> 9;
}
physical_addr pte_frame_base(pt_entry entry) {
	return (entry & PTE_FRAME_BASE) >> 12;
}
void pte_set_frame(pt_entry* entry, physical_addr addr) {
	*entry = (*entry & ~PTE_FRAME_BASE) | (addr << 12);
}

bool pde_is_present(pd_entry entry) { return entry & PDE_PRESENT; }
bool pde_is_available(pd_entry entry) {
	return (entry & PDE_IS_AVAILABLE) >> 9;
}
physical_addr pde_frame_base(pd_entry entry) {
	return (entry & PDE_FRAME_BASE) >> 12;
}
void pde_set_frame(pd_entry* entry, physical_addr addr) {
	*entry = (*entry & ~PDE_FRAME_BASE) | (addr << 12);
}

bool vmem_alloc_page(pt_entry* e) {
	void* page = pmem_alloc_block();
	if (page == NULL) return false;

	pte_set_frame(e, (physical_addr)page);
	pte_set_attrib(e, PTE_PRESENT);
	return true;
}

void vmem_free_page(pt_entry* e) {
	void* page = (void*)pte_frame_base(*e);
	if (page != NULL) pmem_free_block(page);
	pte_unset_attrib(e, PTE_PRESENT);
}

pt_entry* vmem_pte_lookup(struct ptable* table, virtual_addr addr) {
	return &table->entries[PAGE_TABLE_INDEX(addr)];
}

pd_entry* vmem_pde_lookup(struct pdirectory* dir, virtual_addr addr) {
	return &dir->entries[PAGE_DIRECTORY_INDEX(addr)];
}

void vmem_init() {}

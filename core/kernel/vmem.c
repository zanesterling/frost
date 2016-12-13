#include "vmem.h"

// LOCAL DEFINITIONS
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)     (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*(x) & 0xfffff000)

#define PTABLE_ADDR_SPACE_SIZE 0x400000
#define DTABLE_ADDR_SPACE_SIZE 0x100000000

#define PAGE_SIZE 4096

struct pdirectory* _cur_directory;
physical_addr _cur_pdbr;

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

bool vmem_switch_pdir(struct pdirectory* dir) {
	if (!dir) return false;

	_cur_directory = dir;
	pmem_set_pdbr(_cur_pdbr);
	return true;
}

struct pdirectory* vmem_get_pdir() {
	return _cur_directory;
}

void vmem_flush_tlb_entry(virtual_addr addr) {
	asm volatile(
		"cli\n"
		"invlpg [%0]\n"
		"sti\n"
		:: "r" (addr)
	);
}

bool vmem_map_page(physical_addr phys, virtual_addr virt) {
	struct pdirectory* pdir = vmem_get_pdir();
	pd_entry* pde = pdir->entries + PAGE_DIRECTORY_INDEX(virt);
	if ((*pde & PTE_PRESENT) != PTE_PRESENT) {
		// page table not present, allocate it
		struct ptable* table = pmem_alloc_block();
		if (!table) return false;
		memset(table, 0, sizeof(struct ptable));

		pd_entry* new_entry = pdir->entries + PAGE_DIRECTORY_INDEX(virt);
		pde_set_attrib(new_entry, PDE_PRESENT);
		pde_set_attrib(new_entry, PDE_READ_WRITE);
		pde_set_frame(new_entry, (physical_addr)table);
	}

	struct ptable* table = (struct ptable*)PAGE_GET_PHYSICAL_ADDRESS(pde);
	pt_entry* page = table->entries + PAGE_TABLE_INDEX(virt);
	pte_set_frame(page, (physical_addr)phys);
	pte_set_attrib(page, PTE_PRESENT);
	return true;
}

bool vmem_init() {
	struct ptable* table_default = pmem_alloc_block();
	if (!table_default) return false;
	memset(table_default, 0, sizeof(struct ptable));

	struct ptable* table_kernel = pmem_alloc_block();
	if (!table_kernel) return false;
	memset(table_kernel, 0, sizeof(struct ptable));

	// first four megs are identity mapped
	for (
		int i = 0, virt = 0, frame = 0;
		i < 1024; // PAGES_PER_TABLE
		i++, frame += 4096, virt += 4096
	) {
		pt_entry page = 0;
		pte_set_attrib(&page, PTE_PRESENT);
		pte_set_frame(&page, frame);
		table_kernel->entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	// map kernel (1mb) to 3gb
	for (
		int i = 0, frame = 0x100000, virt = 0xc0000000;
		i < 1024; // PAGES_PER_TABLE
		i++, frame += 4096, virt += 4096
	) {
		pt_entry page = 0;
		pte_set_attrib(&page, PTE_PRESENT);
		pte_set_frame(&page, frame);
		table_kernel->entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	struct pdirectory* pdir = pmem_alloc_block();
	if (!pdir) return false;
	memset(pdir, 0, sizeof(struct pdirectory));

	pd_entry* entry_default = pdir->entries + PAGE_DIRECTORY_INDEX(0x0);
	pde_set_attrib(entry_default, PDE_PRESENT);
	pde_set_attrib(entry_default, PDE_READ_WRITE);
	pde_set_frame(entry_default, (physical_addr)table_default);

	pd_entry* entry_kernel = pdir->entries + PAGE_DIRECTORY_INDEX(0xc0000000);
	pde_set_attrib(entry_kernel, PDE_PRESENT);
	pde_set_attrib(entry_kernel, PDE_READ_WRITE);
	pde_set_frame(entry_kernel, (physical_addr)table_kernel);

	_cur_pdbr = (physical_addr) &pdir->entries;
	vmem_switch_pdir(pdir);
	pmem_paging_enable(true);

	return true;
}

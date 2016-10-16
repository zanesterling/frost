#include "main.h"

int kernel_main(multiboot_info* info) {
	puts("Hello, world of kernels\n\n");
	int init_err = init(info);
	if (init_err) {
		printf("init_err: %x\n", init_err);
	} else {
		run_shell();
		puts("\nThanks for using FrOSt\n");
	}

	asm(
		"cli\n"
		"hlt\n"
	);
	return 0;
}

int init(multiboot_info* bootinfo) {
	int hal_err = hal_initialize(bootinfo);
	if (hal_err) return hal_err;

	install_handlers();
	enable_interrupts();
	kbrd_install(33);

	// initialize physical memory
	uint64_t mem_size = get_memory_size_kb(bootinfo);
	struct mem_map memory_map = (struct mem_map) {
		bootinfo->mmap_length,
		(struct mmap_entry*) bootinfo->mmap_addr
	};
	void* bitmap_location = (void*) 0x1000;
	pmem_init(mem_size, bitmap_location, memory_map);

	return 0;
}

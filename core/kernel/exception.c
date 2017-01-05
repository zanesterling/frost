#include "exception.h"

void divide_by_zero_fault() {
	kernel_panic("Divide by 0");
	for (;;);
}

void single_step_trap() {
	kernel_panic("Single step trap");
	for(;;);
}

void nmi_trap() {
	kernel_panic("NMI trap");
	for(;;);
}

void breakpoint_trap() {
	kernel_panic("Breakpoint trap");
	for(;;);
}

void overflow_trap() {
	kernel_panic("Overflow trap");
	for(;;);
}

void bounds_check_fault() {
	kernel_panic("Bounds check fault");
	for(;;);
}

void invalid_opcode_fault() {
	kernel_panic("Invalid opcode");
	for(;;);
}

void no_device_fault() {
	kernel_panic("No device fault");
	for(;;);
}

void double_fault_abort() {
	kernel_panic("Double fault abort");
	for(;;);
}

void invalid_tss_fault() {
	kernel_panic("Invalid TSS fault");
	for(;;);
}

void no_segment_fault() {
	kernel_panic("No segment fault");
	for(;;);
}

void stack_fault() {
	kernel_panic("Stack fault");
	for(;;);
}

void general_protection_fault() {
	kernel_panic("General Protection Fault *salutes*");
	for(;;);
}

void page_fault() {
	kernel_panic("Page fault");
	for(;;);
}

void fpu_fault() {
	kernel_panic("FPU fault");
	for(;;);
}

void alignment_check_fault() {
	kernel_panic("Alignment check fault");
	for(;;);
}

void machine_check_abort() {
	kernel_panic("Machine check abort");
	for(;;);
}

void simd_fpu_fault() {
	kernel_panic("SIMD FPU fault");
	for(;;);
}

void install_handlers() {
	setvect (0, divide_by_zero_fault);
	setvect (1, single_step_trap);
	setvect (2, nmi_trap);
	setvect (3, breakpoint_trap);
	setvect (4, overflow_trap);
	setvect (5, bounds_check_fault);
	setvect (6, invalid_opcode_fault);
	setvect (7, no_device_fault);
	setvect (8, double_fault_abort);
	setvect (10, invalid_tss_fault);
	setvect (11, no_segment_fault);
	setvect (12, stack_fault);
	setvect (13, general_protection_fault);
	setvect (14, page_fault);
	setvect (16, fpu_fault);
	setvect (17, alignment_check_fault);
	setvect (18, machine_check_abort);
	setvect (19, simd_fpu_fault);

	init_scheduler();
	setvect(32, scheduler_tick_handler);
}

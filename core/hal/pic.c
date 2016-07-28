#include "pic.h"

#include "hal.h"
#include "idt.h"

void io_wait() {
	asm volatile(
		"jmp 1f\n"
		"1: jmp 2f\n"
		"2:"
	);
}

void i86_pic_initialize() {
	uint8_t a = i86_pic_read_data(0);
	uint8_t b = i86_pic_read_data(1);

	// ICW 1
	i86_pic_send_command(0x11, 0);
	i86_pic_send_command(0x11, 1);

	// ICW 2
	i86_pic_send_data(0x20, 0);
	i86_pic_send_data(0x28, 1);

	// ICW 3
	i86_pic_send_data(0x4, 0);
	i86_pic_send_data(0x2, 1);

	// ICW4
	i86_pic_send_data(0x1, 0);
	i86_pic_send_data(0x1, 1);

	// restore masks
	i86_pic_send_data(a, 0);
	i86_pic_send_data(b, 1);
}

inline void i86_pic_send_command(uint8_t cmd, uint8_t pic) {
	if (pic > 1) {
		return;
	}

	uint8_t reg = pic ? I86_PIC2_PORT_COMMAND : I86_PIC_PORT_COMMAND;
	outbyte(reg, cmd);
}

inline void i86_pic_send_data(uint8_t data, uint8_t pic) {
	if (pic > 1) {
		return;
	}

	uint8_t reg = pic ? I86_PIC2_PORT_DATA_REG : I86_PIC_PORT_DATA_REG;
	outbyte(reg, data);
}

inline int16_t i86_pic_read_data(uint8_t pic) {
	if (pic > 1) {
		return -1;
	}

	uint8_t reg = pic ? I86_PIC2_PORT_DATA_REG : I86_PIC_PORT_DATA_REG;
	return inbyte(reg);
}

inline void sendeoi() {
	outbyte(I86_PIC_PORT_COMMAND, 0x20);
}

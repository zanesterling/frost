#include "pic.h"

int i86_pic_initialize() {
	// ICW 1
	i86_pic_send_command(0x11, 0);
	i86_pic_send_command(0x11, 1);

	// ICW 2
	i86_pic_send_command(0x20, 0);
	i86_pic_send_command(0x28, 1);

	// ICW 3
	i86_pic_send_command(0x4, 0);
	i86_pic_send_command(0x2, 1);

	// ICW4
	i86_pic_send_command(0x1, 0);
	i86_pic_send_command(0x1, 1);

	return 0;
}

inline void sendeoi() {
	outbyte(PIC_PORT_COMMAND, 0x20);
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

inline uint8_t i86_pic_read_data(uint8_t pic) {
	if (pic > 1) {
		return;
	}

	uint8_t reg = pic ? I86_PIC2_PORT_DATA_REG : I86_PIC_PORT_DATA_REG;
	inbyte(reg);
}

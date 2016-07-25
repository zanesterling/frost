#include "pic.h"

int i86_pic_initialize() {
	// ICW 1
	outbyte(PIC_PORT_COMMAND,  0x11);
	outbyte(PIC2_PORT_COMMAND, 0x11);

	// ICW 2
	outbyte(PIC_PORT_DATA_REG,  0x20);
	outbyte(PIC2_PORT_DATA_REG, 0x28);

	// ICW 3
	outbyte(PIC_PORT_DATA_REG,  0x4);
	outbyte(PIC2_PORT_DATA_REG, 0x2);

	// ICW4
	outbyte(PIC_PORT_DATA_REG,  0x1);
	outbyte(PIC2_PORT_DATA_REG, 0x1);

	// Null the data registers (unclear if necessary)
	outbyte(PIC_PORT_DATA_REG,  0x0);
	outbyte(PIC2_PORT_DATA_REG, 0x0);

	return 0;
}

void sendeoi() {
	outbyte(PIC_PORT_COMMAND, 0x20);
}

#include "keyboard.h"

enum KBRD_CTRL_STATS_MASK {
	KBRD_CTRL_STATS_MASK_OUT_BUF  = 0x01,
	KBRD_CTRL_STATS_MASK_IN_BUF   = 0x02,
	KBRD_CTRL_STATS_MASK_SYSTEM   = 0x04,
	KBRD_CTRL_STATS_MASK_CMD_DATA = 0x08,
	KBRD_CTRL_STATS_MASK_LOCKED   = 0x10,
	KBRD_CTRL_STATS_MASK_AUX_BUF  = 0x20,
	KBRD_CTRL_STATS_MASK_TIMEOUT  = 0x40,
	KBRD_CTRL_STATS_MASK_PARITY   = 0x80,
};

enum KBRD_CTRL_CMD {
	KBRD_CTRL_CMD_READ_CMD          = 0x20,
	KBRD_CTRL_CMD_WRITE_CMD         = 0x60,
	KBRD_CTRL_CMD_SELF_TEST         = 0xaa,
	KBRD_CTRL_CMD_INTERFACE_TEST    = 0xab,
	KBRD_CTRL_CMD_DISABLE_KEYBOARD  = 0xad,
	KBRD_CTRL_CMD_ENABLE_KEYBOARD   = 0xae,
	KBRD_CTRL_CMD_READ_INPUT_PORT   = 0xc0,
	KBRD_CTRL_CMD_READ_OUTPUT_PORT  = 0xd0,
	KBRD_CTRL_CMD_WRITE_OUTPUT_PORT = 0xd1,
	KBRD_CTRL_CMD_READ_TEST_INPUTS  = 0xe0,
	KBRD_CTRL_CMD_SYSTEM_RESET      = 0xfe,
	KBRD_CTRL_CMD_DISABLE_MOUSE     = 0xa7,
	KBRD_CTRL_CMD_ENABLE_MOUSE      = 0xa8,
	KBRD_CTRL_CMD_TEST_MOUSE        = 0xa9,
	KBRD_CTRL_CMD_WRITE_TO_MOUSE    = 0xd4,
};

enum KBRD_ENC_CMD {
	KBRD_ENC_CMD_SET_LED               = 0xed,
	KBRD_ENC_CMD_ECHO                  = 0xee,
	KBRD_ENC_CMD_SET_ALT_SCANCODE_SET  = 0xf0,
	KBRD_ENC_CMD_QUERY_KEYBOARD_ID     = 0xf2,
	KBRD_ENC_CMD_SET_AR_DELAY          = 0xf3,
	KBRD_ENC_CMD_ENABLE_KEYBOARD       = 0xf4,
	KBRD_ENC_CMD_RESET_AND_DISABLE     = 0xf5,
	KBRD_ENC_CMD_RESET_AND_ENABLE      = 0xf6,
	KBRD_ENC_CMD_SET_AR_ALL            = 0xf7,
	KBRD_ENC_CMD_SET_MAKE_BREAK_ALL    = 0xf8,
	KBRD_ENC_CMD_SET_MAKE_ALL          = 0xf9,
	KBRD_ENC_CMD_SET_AR_MAKE_BREAK_ALL = 0xfa,
	KBRD_ENC_CMD_SET_AR_ONE            = 0xfb,
	KBRD_ENC_CMD_SET_MAKE_BREAK_ONE    = 0xfc,
	KBRD_ENC_CMD_SET_MAKE_ONE          = 0xfd,
	KBRD_ENC_CMD_RESEND_LAST           = 0xfe,
	KBRD_ENC_CMD_RESET_AND_SELF_TEST   = 0xff,
};

/* Low level send and read funcs */
uint8_t kbrd_ctrl_read_status() {
	return inbyte(0x64);
}

void kbrd_ctrl_send_cmd(uint8_t cmd) {
	while (1) {
		if ((kbrd_ctrl_read_status() & KBRD_CTRL_STATS_MASK_IN_BUF) == 0) {
			break;
		}
	}

	outbyte(0x64, cmd);
}

uint8_t kbrd_enc_read_buf() {
	return inbyte(0x60);
}

void kbrd_enc_send_cmd(uint8_t cmd) {
	while (1) {
		if ((kbrd_ctrl_read_status() & KBRD_CTRL_STATS_MASK_IN_BUF) == 0) {
			break;
		}
	}

	outbyte(0x60, cmd);
}


/* Initialization funcs */
uint8_t kbrd_self_test() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_SELF_TEST);
	while (1) {
		if (kbrd_ctrl_read_status() & KBRD_CTRL_STATS_MASK_OUT_BUF) {
			break;
		}
	}

	return kbrd_enc_read_buf() == 0x55;
}

uint8_t kbrd_interface_test() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_INTERFACE_TEST);
	return kbrd_enc_read_buf() == 0;
}

void kbrd_disable() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_DISABLE_KEYBOARD);
	_kbrd_is_enabled = 0;
}

void kbrd_enable() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_ENABLE_KEYBOARD);
	_kbrd_is_enabled = 1;
}


/* Client library */
uint8_t kbrd_read_input() {}
uint8_t kbrd_read_output();
void kbrd_write_output(uint8_t);


/* Testing */
uint8_t kbrd_read_test_input();


/* Termination */
void kbrd_system_reset();


/* Uncategorized */
void kbrd_set_leds(uint8 num_lock, uint8 caps_lock, uint8 scroll_lock) {
	uint8 data = 0;
	data = scroll_lock ? (data | 1) : (data & 1);
	data = num_lock ? (data | 2) : (data & 2);
	data = caps_lock ? (data | 4) : (data & 4);

	kbrd_enc_send_cmd(KBRD_ENC_CMD_SET_LED);
	kbrd_enc_send_cmd(data);
}

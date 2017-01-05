#include "keyboard.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS
//============================================================================

// keyboard controller ----------------
enum KBRD_CTRL_IO {
	KBRD_CTRL_STATS_REG = 0x64,
	KBRD_CTRL_CMD_REG   = 0x64,
};

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

// keyboard encoder ----------------
enum KBRD_ENC_IO {
	KBRD_ENC_INPUT_BUF = 0x60,
	KBRD_ENC_CMD_BUF   = 0x60,
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

// scan error codes ----------------
enum KBRD_ERROR {
	KBRD_ERR_BUF_OVERRUN           = 0,
    KBRD_ERR_ID_RET                = 0x83AB,
    KBRD_ERR_BAT                   = 0xAA,
    KBRD_ERR_ECHO_RET              = 0xEE,
    KBRD_ERR_ACK                   = 0xFA,
    KBRD_ERR_BAT_FAILED            = 0xFC,
    KBRD_ERR_DIAG_FAILED           = 0xFD,
    KBRD_ERR_RESEND_CMD            = 0xFE,
    KBRD_ERR_KEY                   = 0xFF
};


//============================================================================
//    IMPLEMENTATION PRIVATE VARS
//============================================================================

uint8_t _kbrd_is_enabled;

// last scancode
#define INVALID_SCANCODE 0
static uint8_t _scancode = INVALID_SCANCODE;

// current state
static bool _numlock, _scrolllock, _capslock;
static bool _shift, _alt, _ctrl;

// set if BAT failed
static bool _kbrd_bat_res = false;

// set if diagnostics failed
static bool _kbrd_diag_res = false;

// set if system should resend last command
static bool _kbrd_resend_res = false;

// map from make code to scan code
static int _kbrd_scancode_std[] = {

	// key			scancode
	KEY_UNKNOWN,	//0
	KEY_ESCAPE,		//1
	KEY_1,			//2
	KEY_2,			//3
	KEY_3,			//4
	KEY_4,			//5
	KEY_5,			//6
	KEY_6,			//7
	KEY_7,			//8
	KEY_8,			//9
	KEY_9,			//0xa
	KEY_0,			//0xb
	KEY_MINUS,		//0xc
	KEY_EQUAL,		//0xd
	KEY_BACKSPACE,	//0xe
	KEY_TAB,		//0xf
	KEY_Q,			//0x10
	KEY_W,			//0x11
	KEY_E,			//0x12
	KEY_R,			//0x13
	KEY_T,			//0x14
	KEY_Y,			//0x15
	KEY_U,			//0x16
	KEY_I,			//0x17
	KEY_O,			//0x18
	KEY_P,			//0x19
	KEY_LEFTBRACKET,//0x1a
	KEY_RIGHTBRACKET,//0x1b
	KEY_RETURN,		//0x1c
	KEY_LCTRL,		//0x1d
	KEY_A,			//0x1e
	KEY_S,			//0x1f
	KEY_D,			//0x20
	KEY_F,			//0x21
	KEY_G,			//0x22
	KEY_H,			//0x23
	KEY_J,			//0x24
	KEY_K,			//0x25
	KEY_L,			//0x26
	KEY_SEMICOLON,	//0x27
	KEY_QUOTE,		//0x28
	KEY_GRAVE,		//0x29
	KEY_LSHIFT,		//0x2a
	KEY_BACKSLASH,	//0x2b
	KEY_Z,			//0x2c
	KEY_X,			//0x2d
	KEY_C,			//0x2e
	KEY_V,			//0x2f
	KEY_B,			//0x30
	KEY_N,			//0x31
	KEY_M,			//0x32
	KEY_COMMA,		//0x33
	KEY_DOT,		//0x34
	KEY_SLASH,		//0x35
	KEY_RSHIFT,		//0x36
	KEY_KP_ASTERISK,//0x37
	KEY_RALT,		//0x38
	KEY_SPACE,		//0x39
	KEY_CAPSLOCK,	//0x3a
	KEY_F1,			//0x3b
	KEY_F2,			//0x3c
	KEY_F3,			//0x3d
	KEY_F4,			//0x3e
	KEY_F5,			//0x3f
	KEY_F6,			//0x40
	KEY_F7,			//0x41
	KEY_F8,			//0x42
	KEY_F9,			//0x43
	KEY_F10,		//0x44
	KEY_KP_NUMLOCK,	//0x45
	KEY_SCROLLLOCK,	//0x46
	KEY_HOME,		//0x47
	KEY_KP_8,		//0x48	//keypad up arrow
	KEY_PAGEUP,		//0x49
	KEY_KP_2,		//0x50	//keypad down arrow
	KEY_KP_3,		//0x51	//keypad page down
	KEY_KP_0,		//0x52	//keypad insert key
	KEY_KP_DECIMAL,	//0x53	//keypad delete key
	KEY_UNKNOWN,	//0x54
	KEY_UNKNOWN,	//0x55
	KEY_UNKNOWN,	//0x56
	KEY_F11,		//0x57
	KEY_F12			//0x58
};

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

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

// keyboard interrupt handler
void i86_kbrd_irq() {
	IRQ_PREFACE();
	asm("cli\n");

	int code = 0;

	if (kbrd_ctrl_read_status() & KBRD_CTRL_STATS_MASK_OUT_BUF) {
		code = kbrd_enc_read_buf();

		if (!(code == 0xe0 || code == 0xe1)) {
			if (code & 0x80) {
				// if it's a break code, convert to make code
				code &= 0x7f;
				int key = _kbrd_scancode_std[code];

				switch (key) {
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = false;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = false;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_alt = false;
						break;
				}
			} else {
				// it's a make code
				int key = _kbrd_scancode_std[code];

				switch (key) {
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = true;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = true;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_alt = true;
						break;

					case KEY_CAPSLOCK:
						_capslock = !_capslock;
						kbrd_set_leds(_numlock, _capslock, _scrolllock);
						break;

					case KEY_KP_NUMLOCK:
						_numlock = !_numlock;
						kbrd_set_leds(_numlock, _capslock, _scrolllock);
						break;

					case KEY_SCROLLLOCK:
						_scrolllock = !_scrolllock;
						kbrd_set_leds(_numlock, _capslock, _scrolllock);
						break;

					default:
						_scancode = code;
				}
			}
		}

		// check for errors
		switch (code) {
			case KBRD_ERR_BAT_FAILED:
				_kbrd_bat_res = false;
				break;

			case KBRD_ERR_DIAG_FAILED:
				_kbrd_diag_res = false;
				break;

			case KBRD_ERR_RESEND_CMD:
				_kbrd_resend_res = false;
				break;
		}
	}

	interruptdone(1);
	IRQ_SUFFIX();
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

char kbrd_key_to_ascii(KEYCODE code) {
	uint8_t key = code;

	if (key <= 0x7f) { // if isascii
		if (_shift != _capslock) {
			if ('a' <= key && key <= 'z') {
				key -= 0x20;
			}
		}

		if (_shift && !_capslock) {
			switch (key) {

				case '0':
					key = KEY_RIGHTPARENTHESIS;
					break;
				case '1':
					key = KEY_EXCLAMATION;
					break;
				case '2':
					key = KEY_AT;
					break;
				case '3':
					key = KEY_EXCLAMATION;
					break;
				case '4':
					key = KEY_HASH;
					break;
				case '5':
					key = KEY_PERCENT;
					break;
				case '6':
					key = KEY_CARRET;
					break;
				case '7':
					key = KEY_AMPERSAND;
					break;
				case '8':
					key = KEY_ASTERISK;
					break;
				case '9':
					key = KEY_LEFTPARENTHESIS;
					break;
				case KEY_COMMA:
					key = KEY_LESS;
					break;

				case KEY_DOT:
					key = KEY_GREATER;
					break;

				case KEY_SLASH:
					key = KEY_QUESTION;
					break;

				case KEY_SEMICOLON:
					key = KEY_COLON;
					break;

				case KEY_QUOTE:
					key = KEY_QUOTEDOUBLE;
					break;

				case KEY_LEFTBRACKET:
					key = KEY_LEFTCURL;
					break;

				case KEY_RIGHTBRACKET:
					key = KEY_RIGHTCURL;
					break;

				case KEY_GRAVE:
					key = KEY_TILDE;
					break;

				case KEY_MINUS:
					key = KEY_UNDERSCORE;
					break;

				case KEY_PLUS:
					key = KEY_EQUAL;
					break;

				case KEY_BACKSLASH:
					key = KEY_BAR;
					break;
			}
		}

		//! return the key
		return key;
	}

	//! scan code != a valid ascii char so no convertion is possible
	return 0;

}

void kbrd_disable() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_DISABLE_KEYBOARD);
	_kbrd_is_enabled = 0;
}

void kbrd_enable() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_ENABLE_KEYBOARD);
	_kbrd_is_enabled = 1;
}

void kbrd_install(uint8_t irq) {
	setvect(irq, i86_kbrd_irq);
}


/* Client library */
uint8_t kbrd_read_input() { return 0; }
uint8_t kbrd_read_output();
void kbrd_write_output(uint8_t);

KEYCODE kbrd_get_last_key() {
	return _scancode == INVALID_SCANCODE
		? KEY_UNKNOWN
		: (KEYCODE)_kbrd_scancode_std[_scancode];
}

void kbrd_discard_last_key() {
	_scancode = INVALID_SCANCODE;
}

/* Testing */
uint8_t kbrd_read_test_input();


/* Reboot */
void kbrd_system_reset() {
	kbrd_ctrl_send_cmd(KBRD_CTRL_CMD_WRITE_OUTPUT_PORT);
	kbrd_enc_send_cmd(0xfe);
}


/* Uncategorized */
void kbrd_set_leds (bool num, bool caps, bool scroll) {
	uint8_t data = 0;

	//! set or clear the bit
	data = (scroll) ? (data | 1) : (data & 1);
	data = (num) ? (num | 2) : (num & 2);
	data = (caps) ? (num | 4) : (num & 4);

	//! send the command -- update keyboard Light Emetting Diods (LEDs)
	kbrd_enc_send_cmd (KBRD_ENC_CMD_SET_LED);
	kbrd_enc_send_cmd (data);
}

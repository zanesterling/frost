#include "scancodes.h"

const char scancodes[0xff] = {
	'\x1b', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '-', '=', '\0', /* backspace */ '\t', 'Q',
	'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',
	'P', '[', ']', '\n', '\0', /* left control */ 'A', 'S', 'D',
	'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',
	'`', '\0', /* left shift	*/ '\\', 'Z', 'X', 'C', 'V', 'B',
	'N',
	'M',
	',',
	'.',
	'/',
	'\0', /* right shift */
	'*', /* (keypad) */
	'\0', /* left alt */
	'\0', /* space */
	'\0', /* CapsLock */
	'\0', /* F1 */
	'\0', /* F2 */
	'\0', /* F3 */
	'\0', /* F4 */
	'\0', /* F5 */
	'\0', /* F6 */
	'\0', /* F7 */
	'\0', /* F8 */
	'\0', /* F9 */
	'\0', /* F10 */
	'\0', /* NumberLock */
	'\0', /* ScrollLock	*/
	'7', /* (keypad) */
	'8', /* (keypad) */
	'9', /* (keypad) */
	'-', /* (keypad) */
	'4', /* (keypad) */
	'5', /* (keypad) */
	'6', /* (keypad) */
	'+', /* (keypad) */
	'1', /* (keypad) */
	'2', /* (keypad) */
	'3', /* (keypad) */
	'0', /* (keypad) */
	'.', /* (keypad) */
	'\0', /* F11 */
	'\0', /* F12 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0',

	'\0', /* escape released */
	'\0', /* 1 released */
	'\0', /* 2 released */
	'\0', /* 3 released */
	'\0', /* 4 released */
	'\0', /* 5 released */
	'\0', /* 6 released */
	'\0', /* 7 released */
	'\0', /* 8 released */
	'\0', /* 9 released */
	'\0', /* 0 (zero) released */
	'\0', /* - released */
	'\0', /* = released */
	'\0', /* backspace released */
	'\0', /* tab released */
	'\0', /* Q released */
	'\0', /* W released */
	'\0', /* E released */
	'\0', /* R released */
	'\0', /* T released */
	'\0', /* Y released */
	'\0', /* U released */
	'\0', /* I released */
	'\0', /* O released */
	'\0', /* P released */
	'\0', /* [ released */
	'\0', /* ] released */
	'\0', /* enter released */
	'\0', /* left control released */
	'\0', /* A released */
	'\0', /* S released */
	'\0', /* D released */
	'\0', /* F released */
	'\0', /* G released */
	'\0', /* H released */
	'\0', /* J released */
	'\0', /* K released */
	'\0', /* L released */
	'\0', /*  ; released */
	'\0', /* ' (single quote) released */
	'\0', /* ` (back tick) released */
	'\0', /* left shift released */
	'\0', /* \ released */
	'\0', /* Z released */
	'\0', /* X released */
	'\0', /* C released */
	'\0', /* V released */
	'\0', /* B released */
	'\0', /* N released */
	'\0', /* M released */
	'\0', /* , released */
	'\0', /* . released */
	'\0', /* / released */
	'\0', /* right shift released */
	'\0', /* (keypad) * released */
	'\0', /* left alt released */
	'\0', /* space released */
	'\0', /* CapsLock released */
	'\0', /* F1 released */
	'\0', /* F2 released */
	'\0', /* F3 released */
	'\0', /* F4 released */
	'\0', /* F5 released */
	'\0', /* F6 released */
	'\0', /* F7 released */
	'\0', /* F8 released */
	'\0', /* F9 released */
	'\0', /* F10 released */
	'\0', /* NumberLock released */
	'\0', /* ScrollLock released */
	'\0', /* (keypad) 7 released */
	'\0', /* (keypad) 8 released */
	'\0', /* (keypad) 9 released */
	'\0', /* (keypad) - released */
	'\0', /* (keypad) 4 released */
	'\0', /* (keypad) 5 released */
	'\0', /* (keypad) 6 released */
	'\0', /* (keypad) + released */
	'\0', /* (keypad) 1 released */
	'\0', /* (keypad) 2 released */
	'\0', /* (keypad) 3 released */
	'\0', /* (keypad) 0 released */
	'\0', /* (keypad) . released */
	'\0',
	'\0',
	'\0',
	'\0', /* F11 released */
	'\0', /* F12 released */

	/*
	0xE0, 0x1C	(keypad) enter pressed	0xE0, 0x1D	right control pressed
	0xE0, 0x35	(keypad) / pressed
	0xE0, 0x38	right alt (or altGr) pressed
	0xE0, 0x47	home pressed
	0xE0, 0x48	cursor up pressed	0xE0, 0x49	page up pressed			0xE0, 0x4B	cursor left pressed
	0xE0, 0x4D	cursor right pressed			0xE0, 0x4F	end pressed
	0xE0, 0x50	cursor down pressed	0xE0, 0x51	page down pressed	0xE0, 0x52	insert pressed	0xE0, 0x53	delete pressed
	0xE0, 0x5B	left GUI pressed
	0xE0, 0x5C	right GUI pressed	0xE0, 0x5D	"apps" pressed
	0xE0, 0x9C	(keypad) enter released	0xE0, 0x9D	right control released
	0xE0, 0xB5	(keypad) / released
	0xE0, 0xB8	right alt (or altGr) released
	0xE0, 0xC7	home released
	0xE0, 0xC8	cursor up released	0xE0, 0xC9	page up released			0xE0, 0xCB	cursor left released
	0xE0, 0xCD	cursor right released			0xE0, 0xCF	end released
	0xE0, 0xD0	cursor down released	0xE0, 0xD1	page down released	0xE0, 0xD2	insert released	0xE0, 0xD3	delete released
	0xE0, 0xDB	left GUI released
	0xE0, 0xDC	right GUI released	0xE0, 0xDD	"apps" released
	0xE0, 0x2A, 0xE0, 0x37	print screen pressed
	0xE0, 0xB7, 0xE0, 0xAA	print screen released
	0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5	pause pressed
	*/
};

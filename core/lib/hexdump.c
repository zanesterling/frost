#include <hexdump.h>

#define ROW_WIDTH 16
#define HALF_ROW (ROW_WIDTH / 2)

void hexdump(void* buf, uint32_t len) {
	char* p = buf;
	for (int row = 0; row < len / ROW_WIDTH; row++) {
		printf("%08x  ", row * ROW_WIDTH);

		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < HALF_ROW; i++) {
				printf("%02x ", p[row * ROW_WIDTH + k * HALF_ROW + i]);
			}
			putch(' ');
		}

		putch(' ');
		putch('|');
		for (int i = 0; i < ROW_WIDTH; i++) {
			char c = p[row * ROW_WIDTH + i];
			if (0x20 <= c && c < 0x7f) putch(c);
			else putch('.');
		}
		putch('|');
		putch('\n');
	}

	// last row
	uint32_t remainder = len % ROW_WIDTH;
	if (remainder > 0) {
		printf("%08x  ", len - remainder);

		hex_out:
		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < HALF_ROW; i++) {
				uint32_t index = row * ROW_WIDTH + k * HALF_ROW + i;
				if (index >= len) break hex_out;
				printf("%02x ", p[row * ROW_WIDTH + k * HALF_ROW + i]);
			}
			putch(' ');
		}

		int neg_remainder = ROW_WIDTH - remainder;
		int to_write = neg_remainder * 3
			+ (neg_remainder < 5 ? 1 : 0)
			+ 1;
		for (; to_write > 0; to_write--) putch(' ');
	}

	printf("%08x\n", len);
}

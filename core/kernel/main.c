#include "main.h"

int main() {
	Puts("Hello, world of kernels\n");
	for (int i = 0; i < 30; i++) {
		Putch('a' + i % 26);
		Putch('\n');
	}
	return 0;
}

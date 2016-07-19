#include "main.h"

int main() {
	printf(
		"Hello, %c%s%c.\nThe ultimate answer is %d (%x in hex).",
		'`',
		"world of kernels",
		'\'',
		42,
		42
	);
	return 0;
}

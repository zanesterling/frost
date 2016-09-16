#include "string.h"

size_t strlen(const char* str) {
	const char* p = str;
	while (*p) p++;
	return p - str;
}

int strcmp(const char* s1, const char* s2) {
	char c1, c2;
	while ((c1 = *s1++) && (c2 = *s2++)) {
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	if (*s1) return 1;
	if (*s2) return -1;

	return 0;
}

void* memcpy(void* dest, void* src, size_t n) {
	uint8* d = (uint8*)dest;
	uint8* s = (uint8*)src;
	for (; n--;) {
		*d++ = *s++;
	}
	return dest;
}

void* memset(void* s, uint8_t x, size_t n) {
	uint8_t* dest = (uint8_t*) s;
	for (; n--;) {
		*dest++ = x;
	}
	return s;
}

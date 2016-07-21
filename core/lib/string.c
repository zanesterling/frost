#include "string.h"

size_t strlen(const char* str) {
	const char* p = str;
	while (*p) p++;
	return p - str;
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

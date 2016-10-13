#include "string.h"

size_t strlen(const char* str) {
	const char* p = str;
	while (*p) p++;
	return p - str;
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 && *s2) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		s1++;
		s2++;
	}

	if (*s1) return 1;
	if (*s2) return -1;

	return 0;
}

char* strncpy(char* dst, const char* src, size_t len) {
	char* p = dst;
	while (len > 0 && *src) {
		*p++ = *src++;
		len--;
	}

	// fill remaining space with null bytes
	while (len-- > 0) *p++ = '\0';

	return dst;
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

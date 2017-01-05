#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "device.h"

#define MAX_FN_LEN 128

typedef uint8_t FILE_NO;
typedef struct FILE {
	DEVICE_NO device;
	DEVICE_TYPE device_type;
	char fn[MAX_FN_LEN];
	bool in_use;
} FILE;


FILE* open(DEVICE_NO device, char* filename);
int read(FILE* file, char* buf, size_t n);
int write(FILE* file, char* buf, size_t n);

void init_file_sys(void* memory);

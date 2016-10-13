#pragma once

#include <stddef.h>
#include <stdint.h>

#define DEVICE_FLOPPY_1    0x00
#define DEVICE_FLOPPY_2    0x01
#define DEVICE_HARD_DISK_1 0x02
#define DEVICE_HARD_DISK_2 0x03

#define MAX_DEVICES 0x100
typedef uint8_t DEVICE_NO;

typedef enum DEVICE_TYPE {
	NONE = 0,
	DEVICE_TYPE_FAT12,
	DEVICE_TYPE_FLOPPY,
} DEVICE_TYPE;

typedef struct DEVICE {
	DEVICE_NO num;
	DEVICE_TYPE type;
} DEVICE;

DEVICE* get_device(DEVICE_NO dev_no);

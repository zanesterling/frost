#include "device.h"

DEVICE* devices = NULL;


DEVICE* get_device(DEVICE_NO dev_no) {
	if (devices == NULL) return NULL;
	return devices + dev_no;
}

void init_devices(void* memory) {
	devices = (DEVICE*) memory;
	devices[DEVICE_FLOPPY_1].type = DEVICE_TYPE_FLOPPY;
	devices[DEVICE_FLOPPY_2].type = DEVICE_TYPE_FLOPPY;

	for (size_t i = 0; i < MAX_DEVICES; i++) {
		devices[i].num = i;
	}
}

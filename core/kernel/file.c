#include "file.h"

#define MAX_FILES UINT8_MAX

FILE* files;


FILE* _get_file(FILE_NO f);
FILE* _get_unused_file();


FILE* open(DEVICE_NO device_num, char* filename) {
	FILE* file = _get_unused_file();
	if (file == NULL) return NULL;

	DEVICE* device = get_device(device_num);
	if (device == NULL) return NULL;

	file->device = device_num;
	file->device_type = get_device(device_num)->type;
	strncpy(file->fn, filename, MAX_FN_LEN);
	file->in_use = true;

	return file;
}

void init_file_sys(void* memory) {
	files = (FILE*) memory;
}


FILE* _get_unused_file() {
	FILE* file;
	for (FILE_NO f_num = 0; f_num < MAX_FILES; f_num++) {
		file = _get_file(f_num);
		if (!file->in_use) break;
	}

	return file;
}

FILE* _get_file(FILE_NO f) {
	return files + f;
}

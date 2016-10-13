#pragma once

#include <hal.h>
#include <stdint.h>
#include <string.h>

#include "spinlock.h"

#define FLOPPY_BYTES_PER_SECTOR 512

enum FloppyRegisters {
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only
   DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7, // write-only
};

void floppy_init_driver();
void floppy_read_sector(void* dst, uint32_t sector_num);

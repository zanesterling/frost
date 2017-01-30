#pragma once

#include <hal.h>
#include <stdint.h>
#include <string.h>

#include "panic.h"
#include "scheduler.h"
#include "spinlock.h"

#define FLOPPY_BYTES_PER_SECTOR 512

enum FloppyRegisters {
   STATUS_REGISTER_A                = 0x00, // read-only
   STATUS_REGISTER_B                = 0x01, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x02,
   TAPE_DRIVE_REGISTER              = 0x03,
   MAIN_STATUS_REGISTER             = 0x04, // read-only
   DATARATE_SELECT_REGISTER         = 0x04, // write-only
   DATA_FIFO                        = 0x05,
   DIGITAL_INPUT_REGISTER           = 0x07, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x07, // write-only
};

void floppy_init_driver();
void floppy_read_sector(void* dst, uint32_t sector_num);

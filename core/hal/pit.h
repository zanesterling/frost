#pragma once

#include <hal.h>

#define I86_PIT_OCW_MASK_BINCOUNT 0x1
#define I86_PIT_OCW_MASK_MODE     0xE
#define I86_PIT_OCW_MASK_RL       0x30
#define I86_PIT_OCW_MASK_COUNTER  0xC0

#define I86_PIT_OCW_BINCOUNT_BINARY    0
#define I86_PIT_OCW_BINCOUNT_BCD       1
#define I86_PIT_OCW_MODE_TERMINALCOUNT 0
#define I86_PIT_OCW_MODE_ONESHOT       0x2
#define I86_PIT_OCW_MODE_RATEGEN       0x4
#define I86_PIT_OCW_MODE_SQUAREWAVEGEN 0x6
#define I86_PIT_OCW_MODE_SOFTWARETRIG  0x8
#define I86_PIT_OCW_MODE_HARDWARETRIG  0xA
#define I86_PIT_OCW_RL_LATCH           0
#define I86_PIT_OCW_RL_LSBONLY         0x10
#define I86_PIT_OCW_RL_MSBONLY         0x20
#define I86_PIT_OCW_RL_DATA            0x30
#define I86_PIT_OCW_COUNTER_0          0
#define I86_PIT_OCW_COUNTER_1          0x40
#define I86_PIT_OCW_COUNTER_2          0x80

void i86_pit_initialize();

void i86_pit_send_command(uint8_t cmd);
void i86_pit_send_data(uint16_t data, uint8_t counter);
uint8_t i86_pit_read_data(uint8_t counter);

void i86_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode);

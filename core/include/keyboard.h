#pragma once

#include "stdint.h"
#include "asm.h"

uint8_t _kbrd_is_enabled;

uint8_t kbrd_ctrl_read_status();
void kbrd_ctrl_send_cmd(uint8_t cmd);
uint8_t kbrd_enc_read_buf();
void kbrd_enc_send_cmd(uint8_t cmd);

uint8_t kbrd_self_test();
uint8_t kbrd_interface_test();
void kbrd_disable();
void kbrd_enable();

uint8_t kbrd_read_input();
uint8_t kbrd_read_output();
void kbrd_write_output(uint8_t);

uint8_t kbrd_read_test_input();
void kbrd_system_reset();

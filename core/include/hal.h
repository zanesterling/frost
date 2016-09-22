#pragma once

#include <bootinfo.h>
#include <stdint.h>

int         hal_initialize(multiboot_info *bootinfo);
int         hal_shutdown();
void        enable_interrupts();
void        disable_interrupts();
void        geninterrupt(uint8_t n);
uint8_t     inbyte(uint16_t port);
void        outbyte(uint16_t port, uint8_t data);
void        setvect(uint8_t interrupt, void (* vect)());
void (*     getvect(uint8_t interrupt) )();
void        interruptdone(uint8_t interrupt);
void        sound(uint8_t freq);
const char* get_cpu_vendor();
uint32_t    get_tick_count();

multiboot_info* get_bootinfo();

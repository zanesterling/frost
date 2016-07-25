#pragma once

#include "stdint.h"

int         hal_initialize();
int         hal_shutdown();
//void        enable();
//void        disable();
void        geninterrupt(uint8_t n);
//void        inportb(uint8_t port);
//uint8_t     outportb(uint8_t port);
void        setvect(uint8_t interrupt, void (* vect)());
//void (*     getvect(uint8_t interrupt) )();
//uint8_t     interruptmask(uint8_t interrupt, uint8_t enable);
void interruptdone(uint8_t interrupt);
//void        sound(uint32_t freq);
//const char* get_cpu_vendor();
//uint32_t    get_tick_count();

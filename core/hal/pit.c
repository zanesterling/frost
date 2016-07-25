#include "pit.h"

#include <hal.h>

#define COUNTER_REG(x) ( \
	(x == 0) ? I86_PIT_REG_COUNTER0 : \
	((x == 1) ? I86_PIT_REG_COUNTER1 : \
	I86_PIT_REG_COUNTER2) \
)

#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND  0x43

uint32_t _pit_ticks = 0;

int i86_pit_initialize();

void set_counter() {
	uint16_t count = 1193180 / 100;
	i86_pit_send_command(0x36);
	i86_pit_send_data(count, 0);
}

void i86_pit_irq() {
	asm(
		"pushad\n"
	);

	_pit_ticks++;
	interruptdone(0);

	asm(
		"popad\n"
		"leave\n"
		"iretd\n"
	);
}

void i86_pit_send_command(uint8_t cmd) {
	outbyte(I86_PIT_REG_COMMAND, cmd);
}

void i86_pit_send_data(uint16_t data, uint8_t counter) {
	if (counter > 2) return;
	outbyte(COUNTER_REG(counter), data & 0xff);
	outbyte(COUNTER_REG(counter), data >> 8);
}

uint8_t i86_pit_read_data(uint8_t counter) {
	if (counter > 2) return 0; // I wish I could assert(0); :(
	return inbyte(COUNTER_REG(counter));
}

void i86_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode) {
	if (counter > 2) return;
	if (freq == 0) return;

	uint8_t ocw = 0;
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	i86_pit_send_command(ocw);

	uint16_t divisor = 1993180 / freq;
	i86_pit_send_data(divisor, 0);

	_pit_ticks = 0;
}

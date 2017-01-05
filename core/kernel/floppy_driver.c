#include "floppy_driver.h"

#define NUM_TRACKS 80
#define BYTES_PER_SECTOR FLOPPY_BYTES_PER_SECTOR
#define SECTORS_PER_TRACK 18
#define HEADS_PER_TRACK 2
#define TOTAL_SECTORS 2880

#define FLOPPY_BASE 0x3f0

// Digital output register bitflags
#define DOR_ENABLE_IRQ_DMA (1 << 3)
#define DOR_NORMAL_MODE    (1 << 2)
#define DOR_RESET_MODE     (0 << 2)
#define DOR_DRIVE_MASK     3

// Main status register bitflags
#define MSR_FIFO_OK         (1 << 7)
#define MSR_FIFO_EXPECTS_IN (1 << 6)
#define MSR_PIO_EXEC_PHASE  (1 << 5)
#define MSR_COMMAND_BUSY    (1 << 4)
#define MSR_DRIVE_3_SEEKING (1 << 3)
#define MSR_DRIVE_2_SEEKING (1 << 2)
#define MSR_DRIVE_1_SEEKING (1 << 1)
#define MSR_DRIVE_0_SEEKING (1 << 0)

// ====== Floppy DMA defines ======
#define DMA_CHANNEL 2

// register ports (chan 0-3)
#define START_ADDR_REG_CHAN_1  0x02
#define COUNT_REG_CHAN_1       0x03
#define START_ADDR_REG_CHAN_2  0x04
#define COUNT_REG_CHAN_2       0x05
#define START_ADDR_REG_CHAN_3  0x06
#define COUNT_REG_CHAN_3       0x07
#define STATUS_REG             0x08
#define COMMAND_REG            0x08
#define REQUEST_REG            0x09
#define SINGLE_CHAN_MASK_REG   0x0a
#define MODE_REG               0x0b
#define FLIPFLOP_RESET_REG     0x0c
#define INTERMEDIATE_REG       0x0d
#define MASTER_RESET_REG       0x0d
#define MASK_RESET_REG         0x0e
#define MULTI_CHAN_MASK_REG    0x0f

SpinLock buflock;
uint8_t buffer[BYTES_PER_SECTOR];


// PRIVATE FUNCTION DEFS
void _lba_to_chs(
	uint32_t lba, uint16_t* track, uint16_t* head, uint16_t* sector
);
uint32_t _chs_to_lba(uint16_t track, uint16_t head, uint16_t sector);

void _prepare_for_dma_read();


// reads a sector into buffer
void _read_sector_chs(
	uint8_t drive, uint16_t track, uint16_t head, uint16_t sector
);

void _floppy_init_dma();


// PUBLIC FUNCTION IMPLS
void floppy_init_driver() {
	//outbyte(, DOR_ENABLE_IRQ_DMA | FLOPPY_DEVICE_2);
	_floppy_init_dma();
	buflock.locked = 0;
}

void floppy_read_sector(void* dst, uint32_t sector_num) {
	uint16_t track, head, sector;
	_lba_to_chs(sector_num, &track, &head, &sector);

	// copy out buffer contents
	lock(&buflock);
		_read_sector_chs(0, track, head, sector);
		memcpy(dst, buffer, BYTES_PER_SECTOR);

		// clear the buf to avoid buf reuse (is this necessary?)
		memset(buffer, 0, BYTES_PER_SECTOR);
	unlock(&buflock);
}


// PRIVATE FUNCTION IMPLS
void _lba_to_chs(
	uint32_t lba, uint16_t* track, uint16_t* head, uint16_t* sector
) {
	*track  = lba / (HEADS_PER_TRACK * SECTORS_PER_TRACK);
	*head   = ((lba % (HEADS_PER_TRACK * SECTORS_PER_TRACK))
		/ SECTORS_PER_TRACK);
	*sector = ((lba % (HEADS_PER_TRACK * SECTORS_PER_TRACK))
		% SECTORS_PER_TRACK + 1);
}

uint32_t _chs_to_lba(uint16_t track, uint16_t head, uint16_t sector) {
	return (track * HEADS_PER_TRACK + head) * SECTORS_PER_TRACK
		+ sector - 1;
}

void _read_sector_chs(
	uint8_t drive, uint16_t track, uint16_t head, uint16_t sector
) {
	// init/reset the controller if needed
	_prepare_for_dma_read();

	// select the drive if needed

	// set up the floppy controller for DMA using the "specify" command
	// seek to the correct cylinder
	// issue a sense interrupt command
	// then issue the standard read/write commands

	// The controller will send an IRQ6 when the transfer is complete
	// Then read the "result" bytes to see if there were any errors
}

void _floppy_init_dma() {
	outbyte(SINGLE_CHAN_MASK_REG, 0x06);
	outbyte(FLIPFLOP_RESET_REG, 0xff);

	outbyte(START_ADDR_REG_CHAN_2, 0); // low byte
	outbyte(START_ADDR_REG_CHAN_2, 0x10); // high byte
	outbyte(FLIPFLOP_RESET_REG, 0xff);

	outbyte(COUNT_REG_CHAN_2, 0xff); // low byte
	outbyte(COUNT_REG_CHAN_2, 0x23); // high byte
	outbyte(0x81, 0);
	outbyte(SINGLE_CHAN_MASK_REG, 0x02);
}

void _prepare_for_dma_read() {
	outbyte(START_ADDR_REG_CHAN_2, 6);
	outbyte(COUNT_REG_CHAN_2, 0x5a);
	outbyte(START_ADDR_REG_CHAN_2, 2);
}

void _prepare_for_dma_write() {
	outbyte(START_ADDR_REG_CHAN_2, 6);
	outbyte(COUNT_REG_CHAN_2, 0x56);
	outbyte(START_ADDR_REG_CHAN_2, 2);
}

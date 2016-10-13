#include "floppy_driver.h"

#define NUM_TRACKS 80
#define BYTES_PER_SECTOR FLOPPY_BYTES_PER_SECTOR
#define SECTORS_PER_TRACK 18
#define HEADS_PER_TRACK 2
#define TOTAL_SECTORS 2880

#define DMA_CHANNEL 2

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

SpinLock buflock;
uint8_t buffer[BYTES_PER_SECTOR];


// PRIVATE FUNCTION DEFS
void _lba_to_chs(
	uint32_t lba, uint16_t* track, uint16_t* head, uint16_t* sector
);
uint32_t _chs_to_lba(uint16_t track, uint16_t head, uint16_t sector);

// reads a sector into buffer
void _read_sector_chs(
	uint8_t drive, uint16_t track, uint16_t head, uint16_t sector
);

void _init_floppy_controller();


// PUBLIC FUNCTION IMPLS
void floppy_init_driver() {
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
	// turn on motor and select drive
	outbyte(
		DIGITAL_OUTPUT_REGISTER,
		(1 << (drive + 4)) | DOR_ENABLE_IRQ_DMA | DOR_NORMAL_MODE
			| (drive & DOR_DRIVE_MASK)
	);

	// wait for motor to get up to speed
	// issue command and parameters to fifo port



	// Issue your command byte plus some parameter bytes (the "command phase") to the FIFO IO port.
	// Exchange data with the drive / "seek" the drive heads (the "execution phase"), on the FIFO IO port.
	// Get an IRQ6 at the end of the execution phase, but only if the command HAS an execution phase.
	// Read any "result bytes" produced by the command (the "result phase"), on the FIFO IO port.
	// The commands "Recalibrate", "Seek", and "Seek Relative" do not have a result phase, and require an additional "Sense Interrupt" command to be sent.
}

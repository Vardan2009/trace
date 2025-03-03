#include "driver/diskio.h"
#include "lib/io.h"
#include "lib/stdio.h"
#include "shell.h"

#define ATA_PRIMARY_IO  0x1F0
#define ATA_PRIMARY_CTRL 0x3F6
#define TIMEOUT 100000

#define ATA_PRIMARY_IO 0x1F0     // Primary controller I/O base port
#define ATA_SECONDARY_IO 0x170   // Secondary controller I/O base port

uint8_t drive = 0;
uint8_t controller = 0;

int drive_num = 0;

void set_drive_controller(int drive_number) {
    drive_num = drive_number;
    controller = drive_number / 2;
    drive = drive_number % 2;
}

void ata_read_sector(uint32_t lba, uint8_t *buffer) {
    // Select the correct I/O base port depending on the controller
    uint16_t base_port = (controller == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    // Select drive (0 for master, 1 for slave)
    uint8_t drive_select = (drive == 0) ? 0xE0 : 0xF0;

    // Set the drive and LBA address
    outb(base_port + 6, drive_select | ((lba >> 24) & 0x0F));
    outb(base_port + 2, 1);  // Number of sectors to read
    outb(base_port + 3, (uint8_t) lba);  // LBA (low byte)
    outb(base_port + 4, (uint8_t) (lba >> 8));  // LBA (middle byte)
    outb(base_port + 5, (uint8_t) (lba >> 16));  // LBA (high byte)
    outb(base_port + 7, 0x20);  // Command to read the sector

    int timeout = TIMEOUT;
    // Wait for the BSY (busy) bit to clear
    while (inb(base_port + 7) & 0x80) {
        if (--timeout <= 0) {
            print_err("Disk read timeout (BSY)!");
            for(;;);
        }
    }

    uint8_t status = inb(base_port + 7);
    if (status & 0x01) {  // Check for error flag
        print_err("Disk error!");
        for(;;);
    }

    timeout = TIMEOUT;
    // Wait for the DRQ (data request) bit to be set
    while (!(inb(base_port + 7) & 0x08)) {
        if (--timeout <= 0) {
            print_err("Disk read timeout (DRQ)!");
            for(;;);
        }
    }

    // Read 256 16-bit words into the buffer
    for (int i = 0; i < 256; i++) {
        ((uint16_t *)buffer)[i] = inw(base_port);  // Read 16-bit words
    }
}

void ata_write_sector(uint32_t lba, uint8_t *buffer) {
    // Select the correct I/O base port depending on the controller
    uint16_t base_port = (controller == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    // Select drive (0 for master, 1 for slave)
    uint8_t drive_select = (drive == 0) ? 0xE0 : 0xF0;

    // Set the drive and LBA address
    outb(base_port + 6, drive_select | ((lba >> 24) & 0x0F));
    outb(base_port + 2, 1);  // Number of sectors to write
    outb(base_port + 3, (uint8_t) lba);  // LBA (low byte)
    outb(base_port + 4, (uint8_t) (lba >> 8));  // LBA (middle byte)
    outb(base_port + 5, (uint8_t) (lba >> 16));  // LBA (high byte)
    outb(base_port + 7, 0x30);  // Command to write a sector

    int timeout = TIMEOUT;
    // Wait for the BSY (busy) bit to clear
    while (inb(base_port + 7) & 0x80) {
        if (--timeout <= 0) {
            print_err("Disk write timeout (BSY)!");
            for(;;);
        }
    }

    uint8_t status = inb(base_port + 7);
    if (status & 0x01) {  // Check for error flag
        print_err("Disk error!\n");
        for(;;);
    }

    timeout = TIMEOUT;
    // Wait for the DRQ (data request) bit to be set
    while (!(inb(base_port + 7) & 0x08)) {
        if (--timeout <= 0) {
            print_err("Disk write timeout (DRQ)!");
            for(;;);
        }
    }

    // Write 256 16-bit words from the buffer
    for (int i = 0; i < 256; i++) {
        outw(base_port, ((uint16_t *)buffer)[i]);  // Write 16-bit words
    }

    // Send the cache flush command to ensure data is written to disk
    outb(base_port + 7, 0xE7);

    timeout = TIMEOUT;
    // Wait for the BSY (busy) bit to clear after flush
    while (inb(base_port + 7) & 0x80) {
        if (--timeout <= 0) {
            print_err("Disk write timeout (FLUSH)!");
            for(;;);
        }
    }
}

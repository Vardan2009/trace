#include "driver/diskio.h"
#include "lib/io.h"
#include "lib/stdio.h"

#define ATA_PRIMARY_IO  0x1F0
#define ATA_PRIMARY_CTRL 0x3F6
#define TIMEOUT 100000

void ata_read_sector(uint32_t lba, uint8_t *buffer) {
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, 1);
    outb(ATA_PRIMARY_IO + 3, (uint8_t) lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t) (lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t) (lba >> 16));
    outb(ATA_PRIMARY_IO + 7, 0x20);

    int timeout = TIMEOUT;
    while (inb(ATA_PRIMARY_IO + 7) & 0x80) {
        if (--timeout <= 0) {
            printf("Disk read timeout (BSY)!\n");
            for(;;);
        }
    }

    uint8_t status = inb(ATA_PRIMARY_IO + 7);
    if (status & 0x01) {  // ERR (Error flag)
        printf("Disk error!\n");
        for(;;);
    }

    timeout = TIMEOUT;
    while (!(inb(ATA_PRIMARY_IO + 7) & 0x08)) {  // DRQ
        if (--timeout <= 0) {
            printf("Disk read timeout (DRQ)!\n");
            for(;;);
        }
    }

    for (int i = 0; i < 256; i++) {
        ((uint16_t *)buffer)[i] = inw(ATA_PRIMARY_IO);  // Read 16-bit words
    }
}
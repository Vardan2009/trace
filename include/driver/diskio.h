#ifndef DISKIO_H
#define DISKIO_H

#include <stdint.h>

void ata_read_sector(uint32_t, uint8_t *);

#endif // DISKIO_H
#include "driver/fs/fat32.h"
#include "driver/diskio.h"
#include "lib/string.h"
#include "lib/malloc.h"

fat32_boot_sector_t *fat32_boot_sector;

void fat32_read_boot_sector() {
	char buffer[512];
    ata_read_sector(0, buffer);
    fat32_boot_sector = (fat32_boot_sector_t*) buffer;
}
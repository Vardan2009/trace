#ifndef ISO9660_H
#define ISO9660_H

#include <stdint.h>

#include "multiboot.h"

void iso9660_initialize(multiboot_info);

uint8_t iso9660_get_boot_drive_number();

#endif  // ISO9660_H
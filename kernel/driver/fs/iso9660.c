#include "driver/fs/iso9660.h"

#include "lib/stdio.h"
#include "multiboot.h"

void iso9660_initialize() {}

uint8_t iso9660_get_boot_drive_number() {
    return (mboot_info.boot_device >> 24) & 0xFF;
}
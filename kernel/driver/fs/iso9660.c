#include "driver/fs/iso9660.h"

#include "lib/stdio.h"

multiboot_info multiboot_inf;

void iso9660_initialize(multiboot_info inf) {
    multiboot_inf = inf;
}

uint8_t iso9660_get_boot_drive_number() {
    return (multiboot_inf.boot_device >> 24) & 0xFF;
}
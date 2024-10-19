#include "mem.h"

void init_memory(multiboot_info* boot_info) {
    int n = 1;
    for (int i = 0; i < boot_info->mmap_length;
         i += sizeof(multiboot_mmap_entry)) {
        multiboot_mmap_entry* mmmt =
            (multiboot_mmap_entry*)(boot_info->mmap_addr + i);

        // printing each entry for now
        printf(
            "--- entry %d ---\n| Low addr: %x\n| High Addr: %x\n| Low Length: "
            "%x\n| High Length: %x\n"
            "| Size: %x\n| Type: %d\n",
            n++, mmmt->addr_low, mmmt->addr_high, mmmt->len_low, mmmt->len_high,
            mmmt->size, mmmt->type);
    }
}
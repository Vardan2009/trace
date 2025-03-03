#include "kernel.h"
#include "lib/fs.h"

multiboot_info mboot_info;

void kernel_main(uint32_t magic, multiboot_info *b_inf) {
    mboot_info = *b_inf;
    init_all();

    set_color_fg(COLOR_AQUA);
    printf(TRACE_LOGO);
    printf("TRACE Operating System (ver. %s)\n\n", TRACE_VER);
    shell();
}

void init_all() {
    set_color_fg(COLOR_DGRAY);
    puts("Initializing TRACE\n");
    printf("Boot Device: 0x%x\n", mboot_info.boot_device);
    init_gdt();
    puts("GDT initialization successful\n");
    init_idt();
    puts("IDT initialization successful\n");
    init_timer();
    puts("Timer initialization successful\n");
    init_keyboard();
    puts("Keyboard driver initialization successful\n");
    uint32_t mod1 = *(uint32_t *)(mboot_info.mods_addr + 4);
    uint32_t phys_alloc_start = (mod1 + 0xfff) & ~0xfff;
    init_memory(mboot_info.mem_upper * 1024, phys_alloc_start);
    init_kmalloc(0x1000);
    init_malloc();
    puts("Memory initialization successful\n");
    serial_init(IOPORT);
    printf("Serial initialization successful on port 0x%x\n", IOPORT);
    init_fs();
    puts("Initialization done.\n");
    set_color_fg(COLOR_WHITE);
}
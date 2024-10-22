#include "kernel.h"

#include "driver/keyboard.h"

extern void syscalls_test();

void kernel_main(uint32_t magic, multiboot_info *boot_info) {
    init_gdt();
    init_idt();
    init_timer();
    init_keyboard();

    uint32_t mod1 = *(uint32_t *)(boot_info->mods_addr + 4);
    uint32_t phys_alloc_start = (mod1 + 0xfff) & ~0xfff;

    init_memory(boot_info->mem_upper * 1024, phys_alloc_start);
    init_kmalloc(0x1000);
    malloc_init();

    serial_init(IOPORT);

    syscalls_test();

    serial_write_str(IOPORT, "Hello Serial!\n");
    printf(
        "Sent message to Serial\n"
        "Received text will be visible here. Keyboard input will be redirected "
        "to "
        "Serial\n-> ");

    set_color_fg(COLOR_LYELLOW);
    while (1) {
        if (serial_received(IOPORT)) putc(serial_read(IOPORT));
        if (!kb_buf_empty(IOPORT)) serial_write(IOPORT, kb_readc());
    }
}
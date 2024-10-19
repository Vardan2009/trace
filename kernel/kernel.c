#include "kernel.h"

#include "driver/keyboard.h"

void kernel_main(uint32_t magic, multiboot_info *boot_info) {
    init_gdt();
    init_idt();
    init_timer();
    init_keyboard();

    // init_memory(boot_info);

    serial_init();

    serial_write_str("Hello, COM1!");
    printf(
        "Sent message to COM1\nListening for data and writing to COM1.\n"
        "Received text will be visible here. Keyboard input will be redirected "
        "to "
        "COM1\n-> ");

    set_color_fg(COLOR_LYELLOW);
    while (1) {
        if (serial_received()) putc(serial_read());
        if (!kb_buf_empty()) serial_write(kb_readc());
    }
}
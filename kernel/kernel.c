#include "kernel.h"

#include "driver/keyboard.h"

void kernel_main(void) {
    printf("Initializing GDT...\n");
    init_gdt();
    printf("Initializing IDT...\n");
    init_idt();
    printf("Initializing PIT...\n");
    init_timer();
    printf("Initializing Keyboard Driver...");
    init_keyboard();

    printf("\nName: ");
    const char *t;
    scanf("%s", t);
    printf("Hello, %s\n", t);

    malloc_init();
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
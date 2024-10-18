#include "kernel.h"
void kernel_main(void) {
    printf("Initializing GDT...\n");
    init_gdt();
    printf("Initializing IDT...\n");
    init_idt();

    malloc_init();
    serial_init();

    const char name[512];
    set_color_fg(COLOR_AQUA);
    printf("What is your name?\n-> ");
    set_color_fg(COLOR_LYELLOW);

    scanf("%s", name);
    set_color_fg(COLOR_AQUA);

    int num;

    printf("Hello, %s! Input an integer\n-> ", name);
    set_color_fg(COLOR_LYELLOW);

    scanf("%d", &num);

    set_color_fg(COLOR_AQUA);
    printf("%d is a cool number\n%d in hex is 0x%x\n", num, num, num);
    set_color_fg(COLOR_WHITE);

    serial_write_str("Hello, COM1!");
    printf(
        "Sent message to COM1\nListening for data and writing to COM1.\n"
        "Received text will be visible here. Keyboard input will be redirected "
        "to "
        "COM1\n-> ");

    set_color_fg(COLOR_LYELLOW);
    while (1) {
        if (serial_received()) putc(serial_read());
        if (kbdata_avail()) serial_write(scank().chr);
    }
}
#include "kernel.h"

void kernel_main(void) {
    malloc_init();
    serial_init();

    const char name[512];
    printf("What is your name?\n-> ");
    scanf("%s", name);

    int age;
    printf("Hello, %s! Input an integer\n-> ");
    scanf("%d", &age);

    printf("%d is a cool number", age);

    serial_write_str("Hello, COM1!");
    while (1)
        if (serial_received()) putc(serial_read());
}
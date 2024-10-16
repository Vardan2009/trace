#include "kernel.h"

void kernel_main(void) {
    malloc_init();
    serial_init();

    char str1[20] = "Hello";
    char str2[20] = "World!";
    char buffer[50];

    strcpy(buffer, str1);
    strcat(buffer, " ");
    strcat(buffer, str2);

    printf("Message: %s\n", buffer);
    printf("The answer is %d\n", 42);
    printf("Unsigned number: %u\n", 12345u);
    printf("Negative number: %d\n", -123);
    printf("Character: %c\n", 'A');
    printf("Hexadecimal: %x\n", 255);

    serial_write_str("Hello, COM1!");
    while(1) {
        if(serial_received())
            putc(serial_read());
    }
}
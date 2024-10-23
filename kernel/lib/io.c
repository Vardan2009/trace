#include "lib/io.h"

#pragma diag_suppress 79
#pragma diag_suppress 20
#pragma diag_suppress 65

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#pragma diag_default 79
#pragma diag_default 20
#pragma diag_defualt 65

int is_transmit_empty(uint16_t port) {
    return inb(port + 5) & 0x20;
}

void serial_write(uint16_t port, char c) {
    while (is_transmit_empty(port) == 0);
    outb(port, c);
}

void serial_write_str(uint16_t port, const char *str) {
    while (*str) serial_write(port, *str++);
}

void serial_init(uint16_t port) {
    // Disable interrupts
    outb(port + 1, 0x00);

    // Enable DLAB (set baud rate divisor)
    outb(port + 3, 0x80);

    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);

    // Disable DLAB, set data bits (8), stop bit (1), no parity
    outb(port + 3, 0x03);

    // Enable FIFO clear them with 14-byte threshold
    outb(port + 2, 0xC7);

    outb(port + 4, 0x0B);
}

int serial_received(uint16_t port) {
    return inb(port + 5) & 1;
}

char serial_read(uint16_t port) {
    while (serial_received(port) == 0);
    return inb(port);
}
#include "lib/io.h"

#pragma diag_suppress 79
#pragma diag_suppress 20
#pragma diag_suppress 65

inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#pragma diag_default 79
#pragma diag_default 20
#pragma diag_defualt 65

int is_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write(char c) {
    while (is_transmit_empty() == 0);
    outb(COM1, c);
}

void serial_write_str(const char *str) {
    while (*str) serial_write(*str++);
}

void serial_init() {
    // Disable interrupts
    outb(COM1 + 1, 0x00);

    // Enable DLAB (set baud rate divisor)
    outb(COM1 + 3, 0x80);

    // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);

    // Disable DLAB, set data bits (8), stop bit (1), no parity
    outb(COM1 + 3, 0x03);

    // Enable FIFO clear them with 14-byte threshold
    outb(COM1 + 2, 0xC7);

    outb(COM1 + 4, 0x0B);
}

int serial_received() {
    return inb(COM1 + 5) & 1;
}

char serial_read() {
    while (serial_received() == 0);
    return inb(COM1);
}
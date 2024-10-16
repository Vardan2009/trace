#ifndef IO_H
#define IO_H

#include <stdint.h>

#define COM1 0x3F8

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

int is_transmit_empty();
void serial_write(char);
void serial_write_str(const char*);

void serial_init();

int serial_received();
char serial_read();

#endif  // IO_H
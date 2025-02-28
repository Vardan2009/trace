#ifndef IO_H
#define IO_H

#include <stdint.h>

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8
#define COM5 0x2e8
#define COM6 0x4f8
#define COM7 0x5e8
#define COM8 0x4e8

void outb(uint16_t, uint8_t);
uint8_t inb(uint16_t);
uint16_t inw(uint16_t);
void outw(uint16_t, uint16_t);

int is_transmit_empty(uint16_t);
void serial_write(uint16_t, char);
void serial_write_str(uint16_t, const char*);

void serial_init(uint16_t);

int serial_received(uint16_t);
char serial_read(uint16_t);

#endif  // IO_H
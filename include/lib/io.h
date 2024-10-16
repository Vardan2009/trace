#ifndef IO_H
#define IO_H

#include <stdint.h>

#define COM1 0x3F8

static inline void outb(uint16_t, uint8_t);
static inline uint8_t inb(uint16_t);

int is_transmit_empty();
void serial_write(char);
void serial_write_str(const char*);

void serial_init();

int serial_received();
char serial_read();

#endif // IO_H
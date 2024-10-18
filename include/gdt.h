#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    unsigned int base;
} __attribute__((packed)) gdt_ptr_t;

void init_gdt();
void set_gdt_gate(uint32_t, uint32_t, uint32_t, uint8_t, uint8_t);

#endif  // GDT_H
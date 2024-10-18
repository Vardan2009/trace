#include "gdt.h"

extern void gdt_flush(addr_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = &gdt_entries;

    // null segment
    set_gdt_gate(0, 0, 0, 0, 0);
    // kernel code segment
    // 9a = 1001 1010
    set_gdt_gate(1, 0, 0xffffffff, 0x9a, 0xcf);
    // kernel data segment
    // 92 = 1001 0010
    set_gdt_gate(2, 0, 0xffffffff, 0x92, 0xcf);
    // user code segment
    // fa = 1111 1010
    set_gdt_gate(3, 0, 0xffffffff, 0xfa, 0xcf);
    // user data segment
    // f2 = 1111 0010
    set_gdt_gate(4, 0, 0xffffffff, 0xf2, 0xcf);

    gdt_flush(&gdt_ptr);
}

void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                  uint8_t gran) {
    gdt_entries[num].base_low = (base & 0xffff);
    gdt_entries[num].base_middle = (base >> 16) & 0xff;
    gdt_entries[num].base_high = (base >> 24) & 0xff;

    gdt_entries[num].limit = (limit & 0xffff);
    gdt_entries[num].flags = (limit >> 16) & 0x0f;
    gdt_entries[num].flags |= (gran & 0xf0);

    gdt_entries[num].access = access;
}
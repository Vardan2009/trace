#include "gdt.h"

extern void gdt_flush(uint32_t);
extern void tss_flush();

gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;
tss_entry_t tss_entry;

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

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
    // task state segment
    write_tss(5, 0x10, 0x0);

    gdt_flush(&gdt_ptr);
    tss_flush();
}

void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    // e9 = 1110 1001
    set_gdt_gate(num, base, limit, 0xe9, 0x00);

    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x08 | 0x3;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs =
        0x10 | 0x3;
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
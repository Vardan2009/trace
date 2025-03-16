#include "driver/pcspk.h"

#include "lib/console.h"
#include "lib/io.h"

void pcspk_beep(uint32_t nFrequence) {
    uint32_t div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(div));
    outb(0x42, (uint8_t)(div >> 8));

    uint32_t tmp = inb(0x61);
    if (tmp != (tmp | 3)) outb(0x61, tmp | 3);
}

void pcspk_stop() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void pcspk_beep_dur(uint32_t freq, uint32_t duration) {
    pcspk_beep(freq);
    delay(duration);
    pcspk_stop();
}

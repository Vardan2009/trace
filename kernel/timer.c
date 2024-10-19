#include "timer.h"

uint64_t ticks;
const uint32_t freq = 100;

void handle_timer(int_regs *regs) {
    ++ticks;
}

void init_timer() {
    ticks = 0;
    irq_install_handler(0, &handle_timer);

    // 119318.1(6) MHz
    uint32_t divisor = 1193180 / freq;

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xff));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xff));
}
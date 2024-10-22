#include "lib/console.h"

void delay(uint32_t ms) {
    // ticks = 100ms / 1000 = 0.1ms
    uint32_t t = ms / 10;
    uint32_t start_ticks = ticks;
    while (ticks - start_ticks < t) asm volatile("nop");
}
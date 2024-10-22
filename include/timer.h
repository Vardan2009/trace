#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "idt.h"
#include "lib/io.h"

extern uint64_t ticks;

void init_timer();
void handle_timer(int_regs*);

#endif  // TIMER_H
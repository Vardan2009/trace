#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "idt.h"
#include "lib/io.h"

void init_timer();
void handle_timer(int_regs*);

#endif  // TIMER_H
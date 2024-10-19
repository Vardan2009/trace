#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "idt.h"
#include "lib/io.h"

void init_keyboard();

char kb_readc();

bool kb_buf_empty();

#endif  // KEYBOARD_H
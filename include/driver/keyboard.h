#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>
#include <stdint.h>

#include "../lib/io.h"

#define KEYS_COUNT 53

typedef struct {
    char chr;
    char shifted_char;
    uint8_t scancode;
} key;

extern key keys[KEYS_COUNT];
extern int shifted;

int kbdata_avail();
uint8_t get_scancode();

key getk(uint8_t);
key scank();

#endif  // KEYBOARD_H
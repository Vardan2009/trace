#include "shell.h"

#include "lib/malloc.h"
#include "lib/stdio.h"

const char *prompt() {
    static char input[512];
    set_color(COLOR_LYELLOW, COLOR_BLACK);
    puts("[-> ");
    set_color_fg(COLOR_GREEN);
    scanl(input, 512);
    set_color_fg(COLOR_WHITE);
    return input;
}

void shell() {
    while (true) { printf("Input: %s\n", prompt()); }
}
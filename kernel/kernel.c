#include "kernel.h"

const char *msg = "Hello!\nWhat? It Works!";

void kernel_main(void) {
    clear_screen();
    set_color(COLOR_LYELLOW, COLOR_BLACK);
    puts(msg);
}
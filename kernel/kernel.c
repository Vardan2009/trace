#include "other.h"

void print_hello() {
    char *vram = (char*) 0xb8000;
    const char *message = "Hello?";
    int i = 0;
    while(message[i]) {
        vram[i * 2] = message[i];
        vram[i * 2 + 1] = 0x07;
        i++;
    }
}

void clear_screen() {
    char *vram = (char*) 0xb8000;
    for(int i =0; i < 80 * 25 * 2; i += 2) {
        vram[i] = ' ';
        vram[i + 1] = 0x07; 
    }
}

void kernel_main(void) {
    clear_screen();
    print_hello();
}
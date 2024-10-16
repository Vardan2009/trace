#include "kernel.h"

void kernel_main(void) {
    malloc_init();
    serial_init();
    
    serial_write_str("Hello, COM1!");
    while(1) {
        if(serial_received())
            putc(serial_read());
    }
}
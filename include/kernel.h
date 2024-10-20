#ifndef KERNEL_H
#define KERNEL_H

#define IOPORT COM1

#include <stdint.h>

#include "gdt.h"
#include "idt.h"
#include "lib/io.h"
#include "lib/kmalloc.h"
#include "lib/stdio.h"
#include "lib/string.h"
#include "mem.h"
#include "multiboot.h"
#include "timer.h"

void kernel_main(uint32_t, multiboot_info*);

#endif  // KERNEL_H
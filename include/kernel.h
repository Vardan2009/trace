#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

#include "gdt.h"
#include "idt.h"
#include "lib/io.h"
#include "lib/mm.h"
#include "lib/stdio.h"
#include "lib/string.h"

void kernel_main(void);

#endif  // KERNEL_H
#ifndef KERNEL_H
#define KERNEL_H

#define TRACE_VER "0.02"

#define IOPORT COM1

#include <stdint.h>

#include "gdt.h"
#include "idt.h"
#include "lib/io.h"
#include "lib/kmalloc.h"
#include "lib/malloc.h"
#include "lib/stdio.h"
#include "lib/string.h"
#include "mem.h"
#include "multiboot.h"
#include "shell.h"
#include "syscalls.h"
#include "timer.h"

extern void syscalls_test();

void kernel_main(uint32_t, multiboot_info*);
void init_all();

#endif  // KERNEL_H
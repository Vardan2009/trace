#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "idt.h"

void handle_syscall(int_regs*);

#endif  // SYSCALLS_H
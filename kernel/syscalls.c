#include "syscalls.h"

void handle_syscall(int_regs* regs) {
    switch (regs->eax) {
        case 0:
            for (int i = 0; i < regs->ecx; i++) putc(((char*)regs->ebx)[i]);
            break;
        case 1:
            while (kb_buf_empty());
            asm volatile("mov %0, %%eax"
                         :
                         : "r"((uint32_t)kb_readc())
                         : "%eax");
            break;
        default:
            curx = cury = 0;
            set_color(COLOR_WHITE, COLOR_RED);
            printf(
                "--- TRACE ---\nException: Invalid Syscall Number %d\nSystem "
                "Halted.",
                regs->eax);
            for (;;);
    }
}
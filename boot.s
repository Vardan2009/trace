section .multiboot
align 4
    dd 0x1badb002
    dd 0x00
    dd -(0x1badb002 + 0x00)

section .text
global _start
extern kernel_main

_start:
    call kernel_main
    hlt
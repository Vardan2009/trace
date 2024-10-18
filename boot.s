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

global gdt_flush
gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret
# TRACE Operating System
TRACE (Targeted Resource Access and Control Engine) is a very simple and minimal operating system written in C for the 32-bit i386 CPU Architecture with the GRUB bootloader. It currently supports GDT, IDT, PIT, has a simple keyboard driver, paging, serial I/O, user I/O, e.t.c. Everything is currently running in Ring 0 (Kernel), but switching to Ring 3 and back will be implemented soon (probably). It also has its own syscalls that can be called with `int 0x80`

# TRACE Syscall Table
| Number | Name      | Arguments                | Description                                      |
|--------|-----------|--------------------------|--------------------------------------------------|
| 0      | `sys_write`| `ebx`: pointer to the buffer <br> `ecx`: number of characters to write | Write N characters from the pointer (buffer) to the screen |
| 1      | `sys_read` |  | Read one character from the input buffer and store in `eax`   |

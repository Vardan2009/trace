# TRACE Operating System
TRACE (Targeted Resource Access and Control Engine) is a very simple and minimal operating system written in C for the 32-bit x86 CPU Architecture with the GRUB bootloader. It currently supports GDT, IDT, PIT, has a simple keyboard driver, paging, serial I/O, user I/O, *very* (currently) buggy FAT32, ISO9660 filesystem drivers, etc. It also has its own syscalls that can be called with `int 0x80`

# Building (Linux)
## Requirements
    
- `gcc`: C Compiler
- `grub-common`: For creating GRUB image
- `grub-pc-bin`
- `xorriso`: Filesystem manipulator
- `make`: Build system

## Optional, but recommended:
- `qemu`: Virtual Machine
- `pulseaudio`: Audio server for `qemu`

You can install the above packages with `apt` or your system's package manager.\
### To build, run the following commands
```sh
sudo make fat32-disk         # Create FAT32 disk with files from copy_to_fat32
sudo make tracefs-disk       # Create empty TRACEFS disk
sudo make iso                # Build ISO
make test-qemu               # Test with QEMU          
```


# TRACE Syscall Table
| Number | Name      | Arguments                | Description                                      |
|--------|-----------|--------------------------|--------------------------------------------------|
| 0      | `sys_write`| `ebx`: pointer to the buffer <br> `ecx`: number of characters to write | Write N characters from the pointer (buffer) to the screen |
| 1      | `sys_read` |  | Read one character from the input buffer and store in `eax`   |

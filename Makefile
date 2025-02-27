CC = gcc
AS = nasm
LD = ld
VM = qemu-system-i386

CFLAGS = -m32 -ffreestanding -nostdlib -Iinclude
ASFLAGS = -felf32
LDFLAGS = -m elf_i386 -T linker.ld
VMFLAGS = -drive file=trace.iso,format=raw -serial mon:stdio -drive file=secondary.iso,format=raw

SRC = $(shell find kernel -name "*.c")
OBJ = $(SRC:.c=.o)

all: kernel.bin

boot.o: boot.s
	$(AS) $(ASFLAGS) -o boot.o boot.s

gdt.o: gdt.s
	$(AS) $(ASFLAGS) -o gdt.o gdt.s

idt.o: idt.s
	$(AS) $(ASFLAGS) -o idt.o idt.s

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

kernel.bin: boot.o gdt.o idt.o $(OBJ)
	$(LD) $(LDFLAGS) -o kernel.bin boot.o gdt.o idt.o $(OBJ)
	mkdir -p iso/boot
	cp kernel.bin iso/boot/kernel.bin

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp -a copy_to_fs/. iso/
	grub-mkrescue -o trace.iso iso/

clean:
	rm -f *.o $(OBJ) kernel.bin
	rm -rf iso
	rm trace.iso

test-qemu:
	$(VM) $(VMFLAGS)

full: iso test-qemu clean

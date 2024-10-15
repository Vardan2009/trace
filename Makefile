CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -Iinclude
ASFLAGS = -felf32
LDFLAGS = -m elf_i386 -T linker.ld
SRC = kernel/kernel.c kernel/other.c
OBJ = $(SRC:.c=.o)

all: kernel.bin

boot.o: boot.s
	$(AS) $(ASFLAGS) -o boot.o boot.s

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

kernel.bin: boot.o $(OBJ)
	$(LD) $(LDFLAGS) -o kernel.bin boot.o $(OBJ)
	mkdir -p iso/boot
	cp kernel.bin iso/boot/kernel.bin

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o trace.iso iso/

clean:
	rm -f *.o kernel/*.o kernel.bin
	rm -rf iso
	rm trace.iso

test-qemu:
	qemu-system-i386 -cdrom trace.iso

full:
	make iso
	make test-qemu
	make clean
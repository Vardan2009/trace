CC = gcc
AS = nasm
LD = ld
VM = qemu-system-i386

CFLAGS = -m32 -ffreestanding -nostdlib -Iinclude
ASFLAGS = -felf32
LDFLAGS = -m elf_i386 -T linker.ld
VMFLAGS = -cdrom trace.iso -serial mon:stdio \
          -drive file=fat32-disk.img,format=raw \
          -drive file=tracefs-disk.img,format=raw \
          -boot order=d

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
	cp -a copy_to_os_fs/. iso/
	grub-mkrescue -o trace.iso iso/

clean:
	rm -f *.o $(OBJ) kernel.bin
	rm -rf iso
	rm trace.iso

test-qemu:
	$(VM) $(VMFLAGS)

tracefs-disk:
	dd if=/dev/zero of=tracefs-disk.img bs=1M count=100
	printf "TRACEFS" | dd of=tracefs-disk.img bs=1 seek=0 conv=notrunc

fat32-disk:
	mkdir -p smnt
	dd if=/dev/zero of=fat32-disk.img bs=1M count=100
	mkfs.fat -F 32 fat32-disk.img
	sudo mount fat32-disk.img smnt
	sudo touch smnt/write.txt
	sudo touch smnt/edit.txt
	sudo mkdir smnt/foldera
	sudo echo "edit me" | sudo tee -a smnt/edit.txt
	sudo umount smnt

full: iso test-qemu clean
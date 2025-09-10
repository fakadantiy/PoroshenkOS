CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -nostdlib

all: kernel.bin

boot.o: boot.s
	as --32 boot.s -o boot.o

kernel.o: kernel.c
	gcc $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: boot.o kernel.o linker.ld
	ld -m elf_i386 $(LDFLAGS) -o kernel.bin boot.o kernel.o

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "Poroshenko OS" { multiboot /boot/kernel.bin }' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o PoroshenkoOS.iso iso

run: iso
	qemu-system-i386 -cdrom PoroshenkoOS.iso

clean:
	rm -rf *.o kernel.bin iso PoroshenkoOS.iso

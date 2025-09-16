gcc -m32 -ffreestanding -c kernel.c -o kernel.o
gcc -m32 -ffreestanding -c libs/vga.c -o vga.o
gcc -m32 -ffreestanding -c libs/keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -c libs/string.c -o string.o
gcc -m32 -ffreestanding -c libs/terminal.c -o terminal.o

as --32 boot.s -o boot.o

ld -m elf_i386 -T linker.ld -nostdlib -o kernel.bin boot.o kernel.o vga.o keyboard.o string.o terminal.o

mkdir -p isodir/boot/grub
cp kernel.bin isodir/boot/kernel.bin

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "POROSHENKOS" {
    multiboot /boot/kernel.bin
}
EOF

grub-mkrescue -o poroshenkos.iso isodir


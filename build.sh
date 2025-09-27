tesl() {
  echo "PoroshenkOS installer: " $1
}
Cc="gcc"
Asm="as"
if command -v grub-mkrescue >/dev/null 2>&1; then
    ISO="env"
fi
if command -v $Cc >/dev/null 2>&1; then
    CCC="good"
fi
if command -v $Asm >/dev/null 2>&1; then
    SAC="scary"
fi
if command -v xorriso >/dev/null 2>&1; then
    WOW="Ebalo"
fi
if command -v mformat >/dev/null 2>&1; then
    WOE="Qbalo"
fi
if [ "$ISO" != "env" ]; then
  tesl "FATAL: no grub-mkrescue"
  exit 1
fi
if [ "$WOW" != "Ebalo" ]; then
  tesl "FATAL: no xorriso"
  exit 1
fi
if [ "$WOE" != "Qbalo" ]; then
  tesl "FATAL: no mformat: install mtools !"
  exit 1
fi
if [ "$CCC" != "good" ]; then
  tesl "FATAL: no $Cc"
  exit 1
fi
if [ "$SAC" != "scary" ]; then
  tesl "FATAL: no $Asm"
  exit 1
fi
if [ "$Cc" != "gcc" ]; then
    tesl "Warning: the CC is not GCC: can be unstable!"
fi
tesl "Compilating [0%]"
$Cc -m32 -ffreestanding -c kernel.c -o kernel.o
tesl "Compilating [20%]"
$Cc -m32 -ffreestanding -c libs/vga.c -o vga.o
tesl "Compilating [40%]"
$Cc -m32 -ffreestanding -c libs/keyboard.c -o keyboard.o
tesl "Compilating [60%]"
$Cc -m32 -ffreestanding -c libs/string.c -o string.o
tesl "Compilating [80%]"
$Cc -m32 -ffreestanding -c libs/terminal.c -o terminal.o
tesl "Compilating [100%]"

$Asm --32 boot.s -o boot.o
tesl "Compilated bootloader"

tesl "Linking"
ld -m elf_i386 -T linker.ld -nostdlib -o kernel.bin boot.o kernel.o vga.o keyboard.o string.o terminal.o
if [ ! -d isodir/boot/grub ]; then
  mkdir -p isodir/boot/grub
fi
tesl "Copying the kernel"
cp kernel.bin isodir/boot/kernel.bin

tesl "Making menuentry for PoroshenkOS"
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "POROSHENKOS" {
    multiboot /boot/kernel.bin
}
EOF
tesl "Making ISO"
grub-mkrescue -o poroshenkos.iso isodir
tesl "You sucessfully built PoroshenkOS"

#!/bin/sh
set -o pipefail
tesl() {
  echo "PoroshenkOS installer: " $1
}

# thx chatgptigga for the tip
# any pc toolchain can build x86 bare-metal btw
IFS=: read -ra dirs <<< "$PATH"
for d in "${dirs[@]}"; do
	for f in "$d"/*-pc-*-gcc; do
		[[ -e $f ]] || continue
		Cc="$f"
		break
	done
done

if command -v grub-rescue >/dev/null 2>&1; then
    ISO="env"
fi
if command -v $Cc >/dev/null 2>&1; then
    CCC="good"
fi
Asm=$($Cc -print-prog-name=as)
if command -v $Asm >/dev/null 2>&1; then
    SAC="scary"
fi
if [ ! -e dev_skip_iso_and_run_qemu ]; then
	if [ "$ISO" != "env" ]; then
	  tesl "FATAL: no grub-rescue"
	  exit 1
	fi
fi
if [ "$CCC" != "good" ]; then
  tesl "FATAL: no $Cc"
  exit 1
fi
if [ "$SAC" != "scary" ]; then
  tesl "FATAL: no $Asm"
  exit 1
fi
#if [ "$Cc" != "gcc" ]; then
#    tesl "Warning: the CC is not GCC: can be unstable!"
#fi
#
tesl "Compilating [0%]"
$Cc -m32 -ffreestanding -march=pentium -fwhole-program -c kernel.c -o kernel.o # flto нельзя(
# -fwhole-program тоже нельзя ибо какой-то молдован писал загрузчик и он нихуя не загрузчик, требует основной код, а на ассемблер -fwhole-program ставить нельзя
# так меня это бесит что по русский заговорил
# так стоп бля, я могу просто аттрибут поставить
# ебать я гений
tesl "Compilating [20%]"
$Cc -m32 -ffreestanding -march=pentium -c libs/vga.c -o vga.o
tesl "Compilating [40%]"
$Cc -m32 -ffreestanding -march=pentium -c libs/keyboard.c -o keyboard.o
tesl "Compilating [60%]"
$Cc -m32 -ffreestanding -march=pentium -c libs/string.c -o string.o
tesl "Compilating [80%]"
$Cc -m32 -ffreestanding -march=pentium -c libs/terminal.c -o terminal.o
tesl "Compilating [100%]"

$Asm --32 boot.s -o boot.o
tesl "Compilated bootloader"

tesl "Linking"
ld -m elf_i386 -T linker.ld -nostdlib -o kernel.bin boot.o kernel.o vga.o keyboard.o string.o terminal.o
if [ ! -d isodir/boot/grub ]; then
  mkdir -p isodir/boot/grub
fi

if [ ! -e dev_skip_iso_and_run_qemu ]; then
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
fi
tesl "You sucessfully built PoroshenkOS"

if [ -e dev_skip_iso_and_run_qemu ]; then
	qemu-system-i386 -machine q35 -m 512 -nodefaults -kernel kernel.bin -display gtk -device virtio-vga -serial none -parallel none -net none
fi
	

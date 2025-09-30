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

if command -v grub-mkrescue >/dev/null 2>&1; then
#if command -v limine >/dev/null 2>&1; then
    ISO="env"
fi
if command -v xxd >/dev/null 2>&1; then
    XXD="Dx"
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
		tesl "FATAL: no grub-mkrescue"
		#tesl "FATAL: no limine"
		exit 1
	fi
	if command -v xorriso >/dev/null 2>&1; then
		WOW="Ebalo"
	fi
	if command -v mformat >/dev/null 2>&1; then
		WOE="Qbalo"
	fi
	if [ "$WOW" != "Ebalo" ]; then
		tesl "FATAL: no xorriso"
		exit 1
	fi
	if [ "$WOE" != "Qbalo" ]; then
		tesl "FATAL: no mformat: install mtools !"
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
if [ "$XXD" != "Dx" ]; then
  tesl "FATAL: no tinyxxd"
  exit 1
fi
#if [ "$Cc" != "gcc" ]; then
#    tesl "Warning: the CC is not GCC: can be unstable!"
#fi
#

tesl "Compiling RoshenLibC [0%]"
#$Cc -m32 -ffreestanding -march=pentium -c libs/RoshenLibC/sys/io.c -o io.o
# так пусто
tesl "Compiling RoshenLibC [100%]"
tesl "Compiling [0%]"
ccache $Cc -m32 -ffreestanding -fno-stack-protector -march=pentium -O3 -mno-80387 -fwhole-program -c kernel.c -o kernel.o # flto нельзя(
# -fwhole-program тоже нельзя ибо какой-то молдован писал загрузчик и он нихуя не загрузчик, требует основной код, а на ассемблер -fwhole-program ставить нельзя
# так меня это бесит что по русский заговорил
# так стоп бля, я могу просто аттрибут поставить
# ебать я гений
# doesn't work without -mno-30387 because interrupt bullshit
# -mno-30387 ISN'T EVEN DOCUMENTED AS A GCC OPTION, WHY
tesl "Compiling [20%]"
ccache $Cc -m32 -ffreestanding -fno-stack-protector -march=pentium -O3 -c libs/vga.c -o vga.o
tesl "Compiling [40%]"
ccache $Cc -m32 -ffreestanding -fno-stack-protector -march=pentium -O3 -c libs/keyboard.c -o keyboard.o
tesl "Compiling [60%]"

# !!! chatgptigga
cd libs
xxd -i commodore64.bin > tmp.h
sed -i \
  -e 's/^unsigned char .*_bin\[\]/unsigned char commodore64[commodore64_size]/' \
  -e '/unsigned int .*_bin_len/d' \
  tmp.h
echo "#define commodore64_size $(stat -c%s commodore64.bin)" | cat - tmp.h > commodore64.h
rm tmp.h
cd ..
tesl "Compiling [80%]"
ccache $Cc -m32 -ffreestanding -fno-stack-protector -march=pentium -O3 -c libs/terminal.c -o terminal.o
tesl "Compiling [100%]"

$Asm --32 boot.s -o boot.o
tesl "Compiled bootloader"

tesl "Linking"
ld -m elf_i386 -T linker.ld -nostdlib -o kernel.bin boot.o kernel.o vga.o keyboard.o terminal.o
if [ ! -d isodir/boot/grub ]; then
  mkdir -p isodir/boot/grub
fi

if [ ! -e dev_skip_iso_and_run_qemu ]; then
#	rm -rf isodir
#	mkdir -p isodir/boot
	tesl "Copying the kernel"
	cp kernel.bin isodir/boot/kernel.bin

	tesl "Making menuentry for PoroshenkOS"
	cat > isodir/boot/grub/grub.cfg << EOF
insmod all_video
insmod gfxterm
insmod gfxterm_background
insmod png
set gfxmode=auto
terminal_output gfxterm
background_image /boot/atb-gordon.png

menuentry "POROSHENKOS" {
	multiboot /boot/kernel.bin
}
EOF
#	cat > isodir/boot/limine.conf << EOF
#wallpaper: boot():/boot/atb-gordon.png
#wallpaper_style: stretched
#
#/PoroshenkOS
#protocol: multiboot
#path: boot():/boot/kernel.bin
#textmode: yes
#EOF

	tesl "Making ISO"
#	cp /usr/share/limine/limine-bios-cd.bin isodir/boot/
#	cp /usr/share/limine/limine-uefi-cd.bin isodir/boot/
#	cp /usr/share/limine/limine-bios.sys isodir/boot/
	cp ATB\ gordon.png isodir/boot/atb-gordon.png
	grub-mkrescue -o poroshenkos.iso isodir
#	xorriso -as mkisofs -R -r -J -b boot/limine-bios-cd.bin \
#	-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
#	-apm-block-size 2048 --efi-boot boot/limine-uefi-cd.bin \
#	-efi-boot-part --efi-boot-image --protective-msdos-label \
#	isodir -o poroshenkos.iso

#	limine bios-install poroshenkos.iso
fi
tesl "You've sucessfully built PoroshenkOS"

if [ -e dev_skip_iso_and_run_qemu ]; then
	qemu-system-i386 -machine q35 -m 512 -nodefaults -kernel kernel.bin -display gtk -device virtio-vga -serial none -parallel none -net none -machine pcspk-audiodev=snd0 -audiodev alsa,id=snd0
fi
	

ISO_NAME = PoroshenkoOS.iso

KERNEL_SRC = kernel.c
VGA_SRC = libs/vga.c
KEYBOARD_SRC = libs/keyboard.c

KERNEL_OBJ = kernel.o
VGA_OBJ = vga.o
KEYBOARD_OBJ = keyboard.o
BOOT_OBJ = boot.o

LD_SCRIPT = linker.ld

CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LD = ld
LDFLAGS = -m elf_i386 -T $(LD_SCRIPT) -nostdlib

GRUB = grub-mkrescue

ISO_DIR = iso

all: $(ISO_NAME)

$(KERNEL_OBJ): $(KERNEL_SRC) libs/vga.h libs/keyboard.h
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)

$(VGA_OBJ): $(VGA_SRC) libs/vga.h
	$(CC) $(CFLAGS) -c $(VGA_SRC) -o $(VGA_OBJ)

$(KEYBOARD_OBJ): $(KEYBOARD_SRC) libs/keyboard.h
	$(CC) $(CFLAGS) -c $(KEYBOARD_SRC) -o $(KEYBOARD_OBJ)

kernel.bin: $(BOOT_OBJ) $(KERNEL_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ) $(LD_SCRIPT)
	$(LD) $(LDFLAGS) -o kernel.bin $(BOOT_OBJ) $(KERNEL_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ)

$(ISO_NAME): kernel.bin
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.bin $(ISO_DIR)/boot/kernel.bin
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "Poroshenko OS" { multiboot /boot/kernel.bin }' >> $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB) -o $(ISO_NAME) $(ISO_DIR)

clean:
	rm -f *.o kernel.bin
	rm -rf $(ISO_DIR)
	rm -f $(ISO_NAME)

.PHONY: all clean

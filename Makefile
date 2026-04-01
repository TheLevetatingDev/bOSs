CC = gcc
LD = ld
AS = as

CFLAGS = -Wall -Wextra -O2 -pipe -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIE -fno-PIC -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -Iinclude
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000 -static

# Automatically finds src/kernel.c, src/requests.c, and src/text.c
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean iso run bin

all: bin kernel.elf

bin:
	$(MAKE) -C bin

# Link all objects into the final kernel
kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# Compile C files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: bin kernel.elf limine.conf
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -v kernel.elf iso_root/boot/
	cp -v limine.conf iso_root/limine.conf
	cp -v bin/limine-bios.sys iso_root/
	cp -v bin/limine-bios-cd.bin bin/limine-uefi-cd.bin iso_root/
	mkdir -p iso_root/EFI/BOOT
	cp -v bin/BOOTX64.EFI bin/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -J -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o boss.iso
	./bin/limine bios-install boss.iso

clean:
	$(MAKE) -C bin clean
	rm -rf kernel.elf $(OBJS) iso_root boss.iso

run: bin iso
	qemu-system-x86_64 -m 1G -M q35 -drive format=raw,file=boss.iso -serial stdio -vga std
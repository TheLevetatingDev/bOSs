CC = gcc
AS = nasm
LD = ld

# Compilation Flags
# Added -Isrc so you can include "modules/graphics/graphics.h" easily
CFLAGS = -Wall -Wextra -O2 -pipe -ffreestanding -fno-stack-protector -fno-stack-check \
         -fno-lto -fno-PIE -fno-PIC -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse \
         -mno-sse2 -mno-red-zone -mcmodel=kernel -Iinclude -Isrc

ASFLAGS = -f elf64
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000 -static

# Find all C and Assembly files recursively in src/
SRCS_C  = $(shell find src -name "*.c")
SRCS_S  = $(shell find src -name "*.s")
OBJS    = $(SRCS_C:.c=.o) $(SRCS_S:.s=.o)

.PHONY: all clean iso run bin

all: kernel.elf

# Only call make in bin if the Makefile exists there
bin:
	@if [ -f bin/Makefile ]; then $(MAKE) -C bin; fi

kernel.elf: $(OBJS)
	@echo " [LD] $@"
	@$(LD) $(LDFLAGS) $(OBJS) -o $@

# Rule for C files
%.o: %.c
	@echo " [CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for Assembly files (nasm)
%.o: %.s
	@echo " [AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

iso: bin kernel.elf limine.conf
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -v kernel.elf iso_root/boot/
	cp -v limine.conf iso_root/
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
	@if [ -f bin/Makefile ]; then $(MAKE) -C bin clean; fi
	rm -rf kernel.elf $(OBJS) iso_root boss.iso

run: iso
	@if [ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ]; then \
        KVM_FLAGS="-enable-kvm -cpu host"; \
    else \
        KVM_FLAGS=""; \
    fi; \
    qemu-system-x86_64 $$KVM_FLAGS \
        -m 1G \
        -M q35 \
        -drive format=raw,file=boss.iso \
        -serial stdio \
        -vga std
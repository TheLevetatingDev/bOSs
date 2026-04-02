# bOSs

bOSs is a hobby operating system kernel built from scratch for x86-64 architecture. It's a minimal, multitasking kernel with basic memory management, scheduling, and hardware support.

## Features

- **Memory Management**: Physical Memory Manager (PMM) with kmalloc heap allocator
- **Multitasking**: Preemptive scheduler with round-robin task switching
- **Interrupt Handling**: Full IDT/PIC setup for hardware and software interrupts
- **Serial I/O**: UART serial console for debugging output
- **Graphics**: Basic text-mode graphics support
- **Timer**: Programmable Interval Timer (PIT) at 100Hz
- **Error Handling**: Kernel panic system with stack dumps

## Dependencies

### Build Dependencies
- `gcc` - C compiler
- `nasm` - Assembler
- `ld` - GNU linker
- `xorriso` - ISO 9660 creation tool
- `make` - Build system

### Runtime Dependencies (for testing)
- `qemu-system-x86_64` - Emulator for running the OS
- KVM (optional, for hardware acceleration)

### Bootloader
The project includes the Limine bootloader binaries in `bin/` directory for both UEFI and BIOS boot support.

## Building

### Build the kernel:
```bash
make
```

### Build a bootable ISO image:
```bash
make iso
```

This creates `boss.iso` with the Limine bootloader configured for both BIOS and UEFI boot.

### Clean build artifacts:
```bash
make clean
```

## Running

### Run in QEMU:
```bash
make run
```

The Makefile will automatically detect KVM availability and use hardware acceleration if available. The QEMU configuration provides:
- 1GB RAM
- Q35 chipset
- Serial output to stdio
- Standard VGA display at 1024x768

### Manual QEMU launch:
```bash
qemu-system-x86_64 -enable-kvm -cpu host -m 1G -M q35 -drive format=raw,file=boss.iso -serial stdio -vga std
```

## Architecture

### Project Structure
```
bOSs/
├── src/                    # Source code
│   ├── kernel.c           # Kernel entry point
│   ├── modules/           # Kernel modules
│   │   ├── sched/         # Task scheduler
│   │   ├── graphics/        # Graphics subsystem
│   │   ├── panic/          # Panic handler
│   │   └── time/           # Timer management
│   ├── mm/                # Memory management
│   │   ├── pmm.c/h         # Physical Memory Manager
│   │   └── kmalloc.c/h     # Kernel heap allocator
│   └── cpu/               # CPU-specific code
│       └── idt.c/h         # Interrupt Descriptor Table
├── include/               # Header files
├── bin/                  # Limine bootloader binaries
├── Makefile             # Build configuration
├── limine.conf          # Limine bootloader config
└── linker.ld            # Linker script
```

### Memory Layout
- Kernel loaded at `0xffffffff80000000` (higher half)
- Pages aligned at 4KB boundaries
- Special handling for Limine request markers

## Development

### Adding New Features
1. Place module code in `src/modules/<module-name>/`
2. Include initialization in `kernel.c`'s `_start()`
3. Update Makefile if adding new source directories
4. Test with `make clean && make && make run`

### Debugging
- Serial output is your best friend
- Panic system provides stack traces
- Use `kprintf()` for logging throughout the code
- QEMU monitor (`Ctrl+Alt+2`) for additional debugging

## Security Notes

This is a hobby project and has not been security audited. Several compiler security features are intentionally disabled for this OS development environment:
- `-fno-stack-protector`
- `-fno-stack-check`
- `-fno-lto`
- `-fno-PIE/-fno-PIC`

Do not use this kernel in production environments.

## License

This project is licensed under the same terms as the Limine bootloader (see `bin/LICENSE`).

## Contributing

This is a personal hobby project, but suggestions and bug fixes are welcome. The primary goal is learning operating system development concepts.

## Acknowledgments

Built with the Limine bootloader (https://github.com/limine-bootloader/limine)

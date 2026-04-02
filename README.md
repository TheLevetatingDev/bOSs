# bOSs - A Hobby x86-64 Kernel

bOSs is a hobbyist operating system kernel built for x86-64 architecture. This is a from-scratch kernel implementing core OS features including memory management, multitasking, and basic I/O.

## Features

- **Memory Management**
  - Physical Memory Manager (PMM)
  - Kernel Memory Allocator (kmalloc)
  - Linked-list based memory management

- **Multitasking**
  - Preemptive multitasking scheduler
  - Round-robin task scheduling
  - Task creation and management API

- **Interrupts & Timing**
  - Interrupt Descriptor Table (IDT)
  - Programmable Interval Timer (PIT) at 100Hz
  - PIC remapping and interrupt handling

- **Graphics & I/O**
  - Basic graphics initialization
  - Serial console output (COM1)
  - Text output capabilities

- **Boot & Build**
  - Limine bootloader support
  - ISO generation for testing
  - QEMU integration for easy testing

## Building

### Prerequisites

- GCC cross-compiler or native GCC with appropriate flags
- NASM assembler
- QEMU (for testing)
- Limine bootloader files (included in `bin/`)
- xorriso (for ISO creation)

### Compiling

```bash
make clean
make
```

### Creating Bootable ISO

```bash
make iso
```

This will create `boss.iso` which can be booted on real hardware or in a VM.

### Running in QEMU

```bash
make run
```

This automatically detects KVM and uses hardware acceleration if available.

## Architecture

### Boot Process

1. Limine bootloader loads the kernel
2. `_start()` handles initialization
3. Core subsystems are initialized in order:
   - Serial I/O
   - Graphics
   - IDT/PIC
   - Timer (PIT)
   - Physical Memory Manager
   - Scheduler
4. Sample tasks are created and interrupts are enabled
5. Kernel enters idle loop

### Code Organization

```
src/
├── kernel.c              # Main kernel entry point
├── cpu/idt.c            # Interrupt Descriptor Table
├── mm/pmm.c             # Physical Memory Manager
├── mm/kmalloc.c         # Kernel heap allocator
├── modules/
│   ├── graphics/        # Graphics output
│   ├── sched/           # Scheduler and task management
│   └── time/            # Timer (PIT) management
├── lib/                 # Utility libraries
└── requests.c           # Limine requests handling

include/                 # Header files
bin/                     # Limine bootloader files
```

### Memory Layout

- Kernel is linked at standard kernel addresses (linker.ld)
- Memory is managed through a linked-list approach
- Kernel uses max-page-size of 0x1000 (4KB pages)

## Current Status

The kernel successfully boots and demonstrates preemptive multitasking with two sample tasks running concurrently. The scheduler uses timer interrupts (100Hz) to switch between tasks.

### Known Issues

- Multitasking is functional but may have edge cases
- Memory management is basic (linked-list only)
- Limited hardware support (primarily QEMU-tested)

## Development

### Adding New Features

1. Place new modules under `src/modules/` or appropriate subdirectories
2. Add headers to `include/` structure
3. Update the Makefile if adding new source files
4. Initialize subsystems in `kernel.c` `_start()` function

### Debugging

- Serial output goes to both QEMU console and COM1
- Use `kprintf()` for debug output
- QEMU monitor available for debugging

## License

This project is currently unlicensed. See individual files for any license headers.

## Contributing

This is a personal hobby project, but suggestions and bug reports are welcome through GitHub issues.

## Acknowledgments

- Limine bootloader for making bootable ISO creation straightforward
- OSDev Wiki and community for kernel development resources
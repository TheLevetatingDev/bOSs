# Changelog

All notable changes to the bOSs kernel project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- Multitasking scheduler with round-robin scheduling
- Task creation and management API
- PIT timer configuration (100Hz)
- Basic memory management (PMM and kmalloc)
- Graphics initialization and text output
- Serial console output via COM1
- Interrupt handling infrastructure (IDT)
- Limine bootloader integration
- ISO generation with QEMU testing support

### Changed
- Restructured source code into modular organization
- Updated build system with cleaner Makefile
- Improved kernel initialization sequence

### Fixed
- Multitasking stability issues
- Timer interrupt handling
- Memory initialization sequence

## [2026-04-01] - Initial Development

### Added
- Basic kernel structure and build system
- Physical Memory Manager (PMM) implementation
- Kernel memory allocator (kmalloc)
- CPU IDT initialization
- Limine bootloader configuration
- Basic linker script

---

## Release Naming Convention

Releases will be named after core feature milestones:
- **v0.1.0** - "Multitasking" (current focus)
- **v0.2.0** - "Virtual Memory" (planned)
- **v0.3.0** - "Device Drivers" (planned)
- **v1.0.0** - "User Mode" (planned)

## Version Tags

Use `git tag` to mark releases:
```bash
git tag -a v0.1.0 -m "Multitasking support"
git push origin v0.1.0
```
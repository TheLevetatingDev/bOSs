# Contributing to bOSs

This is a hobby project, but contributions are welcome! Here are some guidelines for contributing.

## Code Style

### C Code

- Use 4 spaces for indentation (no tabs)
- Opening braces on the same line for functions and control structures
- Use `snake_case` for functions and variables
- Use `UPPER_CASE` for macros and constants
- Add comments explaining complex logic
- Keep lines under 100 characters where possible

Example:
```c
void function_name(int parameter) {
    if (condition) {
        do_something();
    }
    
    return result;
}
```

### Assembly

- Use NASM syntax
- Comment each section
- Document register usage

## Commit Messages

Use clear, descriptive commit messages:

- Prefix with subsystem name: `sched:`, `mm:`, `graphics:`, etc.
- Keep subject line under 50 characters
- Provide detailed description in body if needed

Examples:
```
sched: fix task switching bug in context restore

mm: add bitmap allocator for improved performance

kernel: initialize serial console before graphics
```

## Pull Request Process

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes
4. Test thoroughly with `make run`
5. Commit with a clear message
6. Push to your fork and create a pull request

## Testing

Always test your changes:

```bash
make clean
make
make iso
make run
```

Check for:
- Kernel boots successfully
- No new warnings during compilation
- Test tasks run properly
- Output appears correctly

## Areas for Improvement

Current areas that could use work:
- Memory allocation algorithms (add buddy allocator)
- More robust task scheduling (priority levels)
- Hardware device drivers
- Virtual memory and paging
- Filesystem implementation
- User mode support
- System calls interface

## Questions?

Feel free to open an issue to discuss any changes before implementing them.
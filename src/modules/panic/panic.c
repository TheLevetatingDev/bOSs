#include "panic.h"
#include "../graphics/graphics.h"
#include <string.h>

#define PANIC_BG_COLOR 0xFF0000  // Red background
#define PANIC_FG_COLOR 0x000000  // Black text

void panic(const char *message) {
    // Disable interrupts to prevent further issues
    __asm__ volatile ("cli");

    // Clear screen with red background
    graphics_clear(PANIC_BG_COLOR);

    // Display "KERNEL PANIC" header
    graphics_write("KERNEL PANIC\n\n", PANIC_FG_COLOR);

    // Display the panic message
    graphics_write(message, PANIC_FG_COLOR);
    graphics_write("\n\n", PANIC_FG_COLOR);

    graphics_write("System halted.\n", PANIC_FG_COLOR);

    // Halt the CPU forever
    while (1) {
        __asm__ volatile ("hlt");
    }
}

// Test function for panic
void test_panic(void) {
    panic("This is a test panic! If you see this message, the panic system is working correctly.");
}

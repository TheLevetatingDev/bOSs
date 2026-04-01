#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

// External declarations
extern uint64_t limine_base_revision[3]; 
extern volatile struct limine_framebuffer_request framebuffer_request;

// Module functions
void kprintf(const char *str); 
void draw_char(char c, uint32_t x, uint32_t y, uint32_t fg);
void init_time(void);
uint64_t get_time_ms(void);

/* Helper to clear a small area of the screen so we can overwrite text */
void clear_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    for (uint32_t i = 0; i < h; i++) {
        uint32_t *dest = (uint32_t *)((uint8_t *)fb->address + (y + i) * fb->pitch + x * 4);
        for (uint32_t j = 0; j < w; j++) {
            dest[j] = 0; // Black
        }
    }
}

/* Minimal function to convert a number to a string since we don't have itoa */
void draw_number(uint64_t n, uint32_t x, uint32_t y) {
    char buf[20];
    int i = 0;
    if (n == 0) buf[i++] = '0';
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    // Draw backwards
    for (int j = i - 1; j >= 0; j--) {
        draw_char(buf[j], x, y, 0xFFFFFFFF);
        x += 8;
    }
}

/* Port I/O functions for Serial */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void init_serial(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

void _start(void) {
    init_serial();
    init_time();
    
    if (!LIMINE_BASE_REVISION_SUPPORTED || framebuffer_request.response == NULL) {
        for (;;) __asm__("hlt");
    }

    kprintf("bOSs Kernel Runtime Console\n");
    kprintf("----------------------------\n");
    kprintf("System Uptime: ");

    // Position where we will draw the live timer (right after "System Uptime: ")
    uint32_t timer_x = 15 * 8; // 15 characters in * 8 pixels wide
    uint32_t timer_y = 16 * 2; // 3rd line (0, 16, 32)

    while (true) {
        uint64_t total_ms = get_time_ms();
        uint64_t sec = total_ms / 1000;
        uint64_t ms = total_ms % 1000;

        // 1. Erase the old time (clear a black box)
        clear_rect(timer_x, timer_y, 150, 16);

        // 2. Draw the new time
        draw_number(sec, timer_x, timer_y);
        draw_char('.', timer_x + 32, timer_y, 0xFFFFFFFF); // Rough estimate for '.' position
        draw_number(ms, timer_x + 40, timer_y);
        
        // 3. Optional: Add a small delay so we aren't flickering too fast
        // In a real OS, you'd wait for a vertical blank (V-Sync)
        for(volatile int i = 0; i < 1000000; i++); 
    }
}
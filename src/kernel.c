#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

// Tell the compiler these are defined in another object file (requests.o)
extern uint64_t limine_base_revision[3]; 

// Use volatile so the compiler knows the bootloader might modify this memory
extern volatile struct limine_framebuffer_request framebuffer_request;

/* Port I/O functions */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Serial Port (COM1) initialization */
static void init_serial(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static int is_transmit_empty(void) {
    return inb(0x3F8 + 5) & 0x20;
}

static void write_serial(char a) {
    while (is_transmit_empty() == 0);
    outb(0x3F8, a);
}

static void kprint(const char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        write_serial(s[i]);
    }
}

static void hcf(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    init_serial();
    kprint("bOSs Kernel Booting (COM1)...\n");

    // Check base revision
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
        kprint("Error: Limine base revision not supported!\n");
        hcf();
    }

    // Check if we received a framebuffer response
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        kprint("Error: No framebuffer available!\n");
        hcf();
    }

    kprint("Framebuffer initialized. Painting the screen...\n");

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    // Paint the screen magenta
    for (uint64_t y = 0; y < fb->height; y++) {
        uint32_t *row = (uint32_t *)((uint8_t *)fb->address + y * fb->pitch);
        for (uint64_t x = 0; x < fb->width; x++) {
            row[x] = 0xFF00FFFF;
        }
    }

    kprint("Screen painted. Welcome to bOSs!\n");
    hcf();
}
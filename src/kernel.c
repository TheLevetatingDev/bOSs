#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

// External declarations from requests.c and text.c
extern uint64_t limine_base_revision[3]; 
extern volatile struct limine_framebuffer_request framebuffer_request;
void kprintf(const char *str); // Defined in src/text.c

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

static void kprint_serial(const char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        while ((inb(0x3F8 + 5) & 0x20) == 0);
        outb(0x3F8, s[i]);
    }
}

static void hcf(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    init_serial();
    
    // 1. Check Limine Revision
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
        kprint_serial("Critcal: Limine revision mismatch\n");
        hcf();
    }

    // 2. Check Framebuffer
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        kprint_serial("Critcal: No framebuffer found\n");
        hcf();
    }

    kprintf("Welcome to bOSs Linux-look-alike!\n");

    // Also mirror to serial so 'make run' shows it in your terminal
    kprint_serial("bOSs Kernel: Framebuffer text initialized.\n");

    hcf();
}
#include "graphics.h"
#include <string.h> // For memmove/memset

// Ensure this matches your requests.c or wherever you defined the request
extern volatile struct limine_framebuffer_request framebuffer_request;

static struct limine_framebuffer *fb;
static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16
#define BG_COLOR    0x000000
#define FG_COLOR    0xFFFFFF

void graphics_init(void) {
    // FIX: Changed .count to .framebuffer_count for newer Limine protocol
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        for (;;) __asm__("hlt");
    }

    fb = framebuffer_request.response->framebuffers[0];
    graphics_clear(BG_COLOR);
}

void graphics_clear(uint32_t color) {
    for (uint64_t y = 0; y < fb->height; y++) {
        uint32_t *dest = (uint32_t *)((uint8_t *)fb->address + y * fb->pitch);
        for (uint64_t x = 0; x < fb->width; x++) {
            dest[x] = color;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

static void scroll(void) {
    void* dst = fb->address;
    void* src = (uint8_t*)fb->address + (fb->pitch * CHAR_HEIGHT);
    size_t size_to_copy = fb->pitch * (fb->height - CHAR_HEIGHT);
    
    // Move existing rows up
    memmove(dst, src, size_to_copy);

    // Wipe the new bottom line
    uint32_t* bottom_line = (uint32_t*)((uint8_t*)fb->address + size_to_copy);
    for(uint64_t i = 0; i < (fb->pitch / 4) * CHAR_HEIGHT; i++) {
        bottom_line[i] = BG_COLOR;
    }

    cursor_y -= CHAR_HEIGHT;
}

void graphics_putc(char c, uint32_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += CHAR_HEIGHT;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        draw_char(c, cursor_x, cursor_y, color);
        cursor_x += CHAR_WIDTH;
    }

    // Horizontal wrap
    if (cursor_x + CHAR_WIDTH > fb->width) {
        cursor_x = 0;
        cursor_y += CHAR_HEIGHT;
    }

    // Vertical scroll
    while (cursor_y + CHAR_HEIGHT > fb->height) {
        scroll();
    }
}

void graphics_write(const char* str, uint32_t color) {
    while (*str) {
        graphics_putc(*str++, color);
    }
}
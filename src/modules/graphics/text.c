#include "limine.h"
#include "font.h"
#include "modules/graphics/graphics.h"
#include <stddef.h>

extern volatile struct limine_framebuffer_request framebuffer_request;

// Track cursor position
static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;

void draw_char(char c, uint32_t x, uint32_t y, uint32_t color) {
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    
    // Get the 8-byte bitmask for the character
    const uint8_t *glyph = font8x8[(uint8_t)c];

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            // Check if the bit at this position is set
            if ((glyph[row] >> (7 - col)) & 1 & 1) {
                // Calculate pixel address: Base + (Y * Pitch) + (X * BytesPerPixel)
                uint32_t *pixel = (uint32_t *)((uint8_t *)fb->address + 
                                  (y + row) * fb->pitch + 
                                  (x + col) * 4);
                *pixel = color;
            }
        }
    }
}

void kprintf(const char *str) {
    uint32_t text_color = 0xFFFFFFFF; // White
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y += 10; // 8 pixels + 2 for spacing
            continue;
        }

        draw_char(str[i], cursor_x, cursor_y, text_color);
        cursor_x += 8;

        // Simple wrap around if we hit the edge
        if (cursor_x + 8 > framebuffer_request.response->framebuffers[0]->width) {
            cursor_x = 0;
            cursor_y += 10;
        }
    }
}
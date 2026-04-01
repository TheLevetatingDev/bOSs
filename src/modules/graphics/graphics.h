#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

void graphics_init(void);
void graphics_clear(uint32_t color);
void graphics_putc(char c, uint32_t color);
void graphics_write(const char* str, uint32_t color);

// Exposed for the existing draw_number if needed
void draw_char(char c, uint32_t x, uint32_t y, uint32_t color);

#endif
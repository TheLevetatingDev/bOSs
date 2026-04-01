#include "limine.h"
#include <stdint.h>

// The base revision
__attribute__((used, section(".requests")))
volatile LIMINE_BASE_REVISION(0);

// START MARKER in its own section
__attribute__((used, section(".requests_start_marker")))
volatile LIMINE_REQUESTS_START_MARKER;

// All requests go in the middle section
__attribute__((used, section(".requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// END MARKER in its own section
__attribute__((used, section(".requests_end_marker")))
volatile LIMINE_REQUESTS_END_MARKER;
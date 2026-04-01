#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

// --- New Subsystem Includes ---
#include "mm/pmm.h"
#include "mm/kmalloc.h"
#include "modules/sched/sched.h"

// External declarations for Limine
extern uint64_t limine_base_revision[3]; 
extern volatile struct limine_framebuffer_request framebuffer_request;

// Module functions (implemented elsewhere)
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

// --- Multitasking Test Tasks ---

void task_a(void) {
    int counter = 0;
    while(1) {
        kprintf("Task A running...\n");
        
        // Draw to screen to show it's alive visually
        clear_rect(10, 50, 100, 16);
        draw_char('A', 10, 50, 0xFF0000FF); // Red 'A'
        draw_number(counter++, 25, 50);

        // Dummy delay so it doesn't spam too fast before you hook up a real timer
        for(volatile int i = 0; i < 20000000; i++); 
        
        // Manually yield the CPU to the next task
        sched_yield(); 
    }
}

void task_b(void) {
    int counter = 0;
    while(1) {
        kprintf("Task B running...\n");
        
        // Draw to screen to show it's alive visually
        clear_rect(10, 70, 100, 16);
        draw_char('B', 10, 70, 0xFF00FF00); // Green 'B'
        draw_number(counter++, 25, 70);

        // Dummy delay
        for(volatile int i = 0; i < 20000000; i++); 
        
        // Manually yield the CPU to the next task
        sched_yield(); 
    }
}

// --- Main Kernel Entry Point ---

void _start(void) {
    // 1. Basic Hardware Init
    init_serial();
    init_time();
    
    // Ensure Limine gave us what we need
    if (!LIMINE_BASE_REVISION_SUPPORTED || framebuffer_request.response == NULL) {
        for (;;) __asm__("hlt");
    }

    kprintf("bOSs Kernel Runtime Console\n");
    kprintf("----------------------------\n");

    // 2. Initialize Memory Management
    kprintf("[*] Initializing Physical Memory Manager...\n");
    pmm_init();

    // 3. Initialize the Scheduler
    kprintf("[*] Initializing Task Scheduler...\n");
    sched_init();

    // 4. Create our user threads (they will use pmm_alloc for their stacks)
    kprintf("[*] Creating Kernel Tasks...\n");
    task_create(task_a);
    task_create(task_b);

    kprintf("[*] Entering Multitasking. Goodbye from main thread!\n");
    kprintf("----------------------------\n");

    // 5. Trigger the first context switch
    // Depending on how you wrote sched_yield, this will save the current 
    // _start state and load task_a's state.
    sched_yield();

    // We should never actually reach this loop if scheduling works, 
    // but it's a safe fallback.
    for (;;) {
        __asm__("hlt");
    }
}
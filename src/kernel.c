#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

/* --- Subsystem Includes --- */
#include "mm/pmm.h"
#include "mm/kmalloc.h"
#include "modules/sched/sched.h"

/* --- Limine Requests --- */
extern uint64_t limine_base_revision[3]; 
extern volatile struct limine_framebuffer_request framebuffer_request;

/* --- External Module Functions --- */
// These should be defined in your drivers/ or text.c
void kprintf(const char *str); 
void draw_char(char c, uint32_t x, uint32_t y, uint32_t fg);
void init_time(void);

/* --- Graphical Helpers --- */

void clear_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    for (uint32_t i = 0; i < h; i++) {
        uint32_t *dest = (uint32_t *)((uint8_t *)fb->address + (y + i) * fb->pitch + x * 4);
        for (uint32_t j = 0; j < w; j++) {
            dest[j] = 0; // Black
        }
    }
}

void draw_number(uint64_t n, uint32_t x, uint32_t y, uint32_t color) {
    char buf[20];
    int i = 0;
    if (n == 0) buf[i++] = '0';
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        draw_char(buf[j], x, y, color);
        x += 8;
    }
}

/* --- Serial I/O --- */

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
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

/* --- Scheduled Worker Tasks --- */

/**
 * task_worker_1: Displays a red counter.
 * This function is 'spawned' and managed by the scheduler.
 */
void task_worker_1(void) {
    uint64_t count = 0;
    while(1) {
        // Log to serial
        // kprintf("[Task 1] Iteration update\n");

        // Update Screen
        clear_rect(20, 100, 150, 16);
        draw_char('1', 20, 100, 0xFFFF0000); // Red
        draw_number(count++, 40, 100, 0xFFFFFFFF);

        // Artificial delay for visibility
        for(volatile int i = 0; i < 15000000; i++); 

        // Hand over control to next task
        sched_yield(); 
    }
}

/**
 * task_worker_2: Displays a green counter.
 */
void task_worker_2(void) {
    uint64_t count = 0;
    while(1) {
        // kprintf("[Task 2] Iteration update\n");

        // Update Screen
        clear_rect(20, 120, 150, 16);
        draw_char('2', 20, 120, 0xFF00FF00); // Green
        draw_number(count++, 40, 120, 0xFFFFFFFF);

        for(volatile int i = 0; i < 15000000; i++); 

        sched_yield(); 
    }
}

/* --- Kernel Entrance --- */

void _start(void) {
    init_serial();
    init_time();
    
    // Safety check for Limine
    if (!LIMINE_BASE_REVISION_SUPPORTED || framebuffer_request.response == NULL) {
        for (;;) __asm__("hlt");
    }

    kprintf("bOSs Kernel: Multitasking Edition\n");
    kprintf("---------------------------------\n");

    // 1. Setup Memory (PMM + Heap)
    kprintf("[SYS] Initializing PMM...\n");
    pmm_init(); 
    
    // 2. Setup Scheduler
    kprintf("[SYS] Initializing Scheduler...\n");
    sched_init();

    // 3. Spawn tasks using your new "do-task" style logic
    // We call these once; the functions themselves contain the while(1)
    kprintf("[SYS] Spawning worker threads...\n");
    task_create(task_worker_1);
    task_create(task_worker_2);

    kprintf("[SYS] Starting Dispatcher Loop...\n");
    kprintf("---------------------------------\n");

    /* The main loop of _start now becomes the "Idle Task".
       Whenever no other task is doing work, the CPU returns here.
    */
    while (1) {
        // yield checks the queue and jumps to task_worker_1 or 2
        sched_yield(); 
        
        // If there are no tasks, we just halt to save power
        __asm__("hlt");
    }
}
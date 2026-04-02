#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"

/* --- Subsystem Includes --- */
#include "mm/pmm.h"
#include "mm/kmalloc.h"
#include "modules/sched/sched.h"
#include "modules/graphics/graphics.h"
#include "modules/time/time.h"  // Ensure this defines delay() and init_time()
#include "cpu/idt.h"            // Added for idt_init()

/* --- External Module Functions --- */
// These should ideally be in headers, but kept here for compatibility with your snippets
extern void kprintf(const char *str);
extern void gdt_init(void); 

/* --- Serial I/O Helpers --- */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void init_serial(void) {
    // Basic UART initialization for COM1 (8N1, 38400 baud approx)
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

/* --- Task Wrapper --- */
void test_task_wrapper(void) {
    while (1) {
        kprintf("Task 1: Running...\n");
        delay(1000);
    }
}

void test_task_2_wrapper(void) {
    while (1) {
        kprintf("Task 2: Running...\n");
        delay(100);
    }
}

/* --- Kernel Entrance --- */

void _start(void) {
    // 1. Core Hardware & CPU State
    init_serial();
    graphics_init();
    
    kprintf("bOSs Kernel: Multitasking Edition\n");
    kprintf("---------------------------------\n");

    // Initialize CPU structures
    // gdt_init();    // Load Global Descriptor Table
    idt_init();       // Load Interrupt Descriptor Table & Remap PIC
    
    // Initialize PIT (Timer)
    init_time();      // Configures PIT to 100Hz
    
    // 2. Setup Memory (PMM + Heap)
    kprintf("[SYS] Initializing PMM...\n");
    pmm_init(); 
    
    // 3. Setup Scheduler
    kprintf("[SYS] Initializing Scheduler...\n");
    sched_init();

    // 4. Create initial tasks
    // Use wrappers to ensure interrupts are enabled in each task
    task_create(test_task_wrapper);
    task_create(test_task_2_wrapper);

    kprintf("[SYS] Enabling Interrupts...\n");
    __asm__ volatile ("sti"); // CRITICAL: Timer won't tick without this

    kprintf("[SYS] Starting Dispatcher Loop...\n");
    kprintf("---------------------------------\n");

    /* * The main loop of _start acts as the "System Idle Task".
     * When no other tasks are ready, the CPU stays here.
     */
    while (1) {
        // Explicitly yield to let test_task run
        sched_yield();
        
        // Halt until the next interrupt (PIT) wakes us up.
        // This saves CPU usage in virtual machines/emulators.
        __asm__ volatile ("hlt");
    }
}
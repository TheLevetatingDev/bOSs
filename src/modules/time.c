#include <stdint.h>
#include <stddef.h>
#include "sched/sched.h"

// --- Constants and Globals ---
#define PIT_FREQUENCY 1193182
#define TARGET_HZ 100 // 100 ticks per second (10ms per tick)

static uint64_t tick_count = 0;

// Tell the compiler these exist elsewhere
extern void sched_yield(); 
extern void kprintf(const char *str);

/* Port I/O Helpers */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * init_time: Configures the PIT to fire at TARGET_HZ
 */
void init_time(void) {
    uint16_t divisor = PIT_FREQUENCY / TARGET_HZ;

    // Command Register: Binary mode, Square Wave, LSB/MSB, Channel 0
    outb(0x43, 0x36);

    // Set the divisor (frequency)
    outb(0x40, (uint8_t)(divisor & 0xFF));        // Low byte
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF)); // High byte

    kprintf("[Time] PIT Initialized to 100Hz\n");
}

/**
 * get_time_ms: Returns elapsed time in milliseconds
 */
uint64_t get_time_ms(void) {
    // Since we set 100Hz, each tick is exactly 10ms
    return tick_count * 10;
}

/**
 * timer_interrupt_handler: Called by your IDT stub when IRQ0 fires.
 */
void timer_interrupt_handler(void) {
    tick_count++;

    /* * PREEMPTION:
     * This is where the magic happens. We force the scheduler to 
     * switch tasks even if the current task didn't call yield().
     */
    if (tick_count % 2 == 0) { // Switch every 20ms
        sched_yield();
    }

    // Send EOI (End of Interrupt) to the Master PIC
    outb(0x20, 0x20);
}
#include "time.h"
#include <stdint.h>
#include <stddef.h>

#define PIT_FREQUENCY 1193182
#define TARGET_HZ 100

static volatile uint64_t tick_count = 0;

extern void sched_yield();
extern void kprintf(const char *str);

// Fixed outb helper
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

void init_time(void) {
    uint16_t divisor = PIT_FREQUENCY / TARGET_HZ;
    outb(0x43, 0x36); // Command: Channel 0, LSB/MSB, Square Wave
    outb(0x40, (uint8_t)(divisor & 0xFF));        // Low byte
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF)); // High byte
}

void delay(uint32_t ms) {
    uint64_t start_ticks = tick_count;
    uint64_t ticks_to_wait = ms / 10;
    if (ms > 0 && ticks_to_wait == 0) ticks_to_wait = 1;

    while ((tick_count - start_ticks) < ticks_to_wait) {
        __asm__ volatile ("pause");
    }
}

void timer_interrupt_handler(void) {
    tick_count++;
    outb(0x20, 0x20);

    sched_yield(); 
}
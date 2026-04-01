#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static uint64_t cpu_freq_hz = 0;
static uint64_t start_tsc = 0;

void init_time(void) {
    // Enable PIT Channel 2
    uint8_t conf = inb(0x61);
    outb(0x61, (conf & 0xFD) | 1);
    outb(0x43, 0xB2);

    // Set PIT to 10ms (11931 cycles)
    outb(0x42, 0x9B); 
    outb(0x42, 0x2E);

    uint64_t tsc_start = rdtsc();
    while (!(inb(0x61) & 0x20));
    uint64_t tsc_end = rdtsc();

    cpu_freq_hz = (tsc_end - tsc_start) * 100;
    start_tsc = rdtsc();
}

// Returns milliseconds since boot
uint64_t get_time_ms(void) {
    if (cpu_freq_hz == 0) return 0;
    
    uint64_t elapsed_cycles = rdtsc() - start_tsc;
    
    // Multiply by 1000 first to maintain precision before dividing
    return (elapsed_cycles * 1000) / cpu_freq_hz;
}
#include "idt.h"
#include <stdint.h>

static struct idt_entry idt[256] __attribute__((aligned(16)));
static struct idt_ptr idtp;

extern void irq0_handler(void);

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

void idt_set_gate(uint8_t vector, void* handler) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].base_low  = addr & 0xFFFF;
    idt[vector].base_mid  = (addr >> 16) & 0xFFFF;
    idt[vector].base_high = (addr >> 32) & 0xFFFFFFFF;
    
    // 0x28 is the Limine 64-bit code selector. 
    // If you wrote your own GDT and it uses 0x08, change this back to 0x08.
    idt[vector].selector  = 0x28; 
    
    idt[vector].flags     = 0x8E; // Present, Ring 0, Interrupt Gate
    idt[vector].ist       = 0;
    idt[vector].reserved  = 0;
}

void idt_init(void) {
    // 1. PIC Remap
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);

    // 2. Mask all except Timer
    outb(0x21, 0xFE); 
    outb(0xA1, 0xFF);

    // 3. Set the Timer Gate
    idt_set_gate(32, irq0_handler);

    // 4. Load IDT
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(idtp));
}
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;      // Kernel Code Segment (0x08)
    uint8_t  ist;           // Interrupt Stack Table (0)
    uint8_t  flags;         // Attributes (0x8E for Interrupt Gate)
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void idt_init(void);
void idt_set_gate(uint8_t vector, void* handler);

#endif
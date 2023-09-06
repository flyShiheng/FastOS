
#include "idt.h"
#include "../driver/vga_print.h"

#define GDT_OFFSET_KERNEL_CODE      (0x01 * 0x08)

idt_desc_t __idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &__idt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = 0x8;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_assemble() {
    idtr.base = (uintptr_t)&__idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_handler, IDT_DESCRIPTOR_EXCEPTION, 0);
    }

    vga_print(" idt_assemble ", TEXT_WHITE);

    // __asm__("lidt %0" : : "m" (idtr));we         
    idt_reload(&idtr);
    __asm__ volatile ("sti");
}

void isr_handler() {
    vga_print("Soft Interrupt message", TEXT_RED);
    __asm__ volatile ("cli; hlt");
}

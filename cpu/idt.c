
#include "idt.h"
#include "../driver/vga_print.h"

#define GDT_OFFSET_KERNEL_CODE      (0x01 * 0x08)

__attribute__((aligned(0x10))) 
idt_desc_t __idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

extern uint64_t isr_stub_table[];

void idt_set_descriptor(uint8_t vector, uint64_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &__idt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = 0x08;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_init() {
    idtr.base = (uint64_t)&__idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, 0);
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}

void isr_handler() {
    printk("\nSoft Interrupt message wow ... \n");
    __asm__ volatile ("cli; hlt");
}


#include "idt.h"
#include "../driver/vga_print.h"

#define GDT_OFFSET_KERNEL_CODE      (0x01 * 0x08)

__attribute__((aligned(0x10))) 
idt_desc_t kidt[IDT_MAX_DESCRIPTORS];

static idtr_t kidtr;

extern uint64_t isr_stub_table[];

char* isr_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void idt_set_descriptor(uint8_t vector, uint64_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &kidt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_init() {
    kidtr.base  = (uint64_t)&kidt[0];
    kidtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, 0);
    }

    __asm__ volatile ("lidt %0" : : "m"(kidtr));
    __asm__ volatile ("sti");
}

void print_frame(isr_frame_t* frame) {
    printk("\nSoft Interrupt isr_frame_t: vector=%d, error=%d, rip=%d, cs=%d, rflags=%d, rsp=%d, dss=%d\n",
        frame->base_frame.vector, frame->base_frame.error_code, frame->base_frame.rip, frame->base_frame.cs,
        frame->base_frame.rflags, frame->base_frame.rsp, frame->base_frame.dss);
}

void isr_handler(isr_frame_t frame) {
    print_frame(&frame);
    __asm__ volatile ("cli; hlt");
}

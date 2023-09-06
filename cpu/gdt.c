#include "gdt.h"

gdt_desc_t __gdt[GDT_MAX_DESCRIPTORS];

static gdtr_t gdtr;

static uint16_t gindex;

void gdt_init() {
    gdtr.limit = (sizeof(gdt_desc_t) * GDT_MAX_DESCRIPTORS) - 1;
    gdtr.base = (uintptr_t)&__gdt[0];

    gdt_add_descriptor(0, 0, 0, 0); // null descriptor for segment registers to have offsets
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY); // kernel code
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR, GDT_BASIC_GRANULARITY); // kernel data
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL, GDT_BASIC_GRANULARITY); // user data
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY); // user code
    gdt_add_descriptor(0, 0, 0, 0); // null descriptor delimiting the TSS

    gdt_reload(&gdtr, GDT_OFFSET_KERNEL_CODE, GDT_OFFSET_KERNEL_DATA);
}

void gdt_add_descriptor(uint64_t base, uint16_t limit, uint8_t access, uint8_t granularity) {
    if (gindex >= GDT_MAX_DESCRIPTORS) 
        return;

    __gdt[gindex].base_low = base & 0xFFFF;
    __gdt[gindex].base_mid = (base >> 16) & 0xFF;
    __gdt[gindex].base_high = (base >> 24) & 0xFF;

    __gdt[gindex].limit = limit;

    __gdt[gindex].flags = access;
    __gdt[gindex].granularity = granularity;

    gindex++;
}

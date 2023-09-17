
#include "./apic.h"

 uint64_t apic_base = 0;

void get_apic_base() {
    asm volatile("rdmsr" : "=a"(apic_base) : "c"(0x1B) : "edx");
    apic_base &= 0xfffff0000;  // APIC基地址在高位
    printk("apic_base: %x\n", apic_base);
}

uint32_t get_apic_id() {
    uint32_t apic_id = *(volatile uint32_t*)(apic_base + APIC_ID);
    return apic_id;
}

uint32_t get_apic_version() {
    uint32_t apic_version = *(volatile uint32_t*)(apic_base + APIC_VERSION);
    return apic_version;
}


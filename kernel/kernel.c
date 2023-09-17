#include "../cpu/idt.h"
#include "../cpu/gdt.h"
#include "../driver/vga_print.h"
#include "../driver/uart.h"
#include "../cpu/apic.h"
#include "../mem/page.h"

void kernel_main() {

    uart_init();

    gdt_init();

    idt_init();

    mem_init();

    printk("\n\nEnter FastOS\n\n");

    // get_apic_base();

    // printk("apic_id: %d, apic_version: %d\n", get_apic_id(), get_apic_version());

    int a = 1 / 0;
    // __asm__ volatile ("int3");
}

#include "../cpu/idt.h"
#include "../cpu/gdt.h"
#include "../driver/vga_print.h"
#include "../driver/uart.h"

void kernel_main() {
    gdt_init();
    idt_init();
    uart_init();

    // __asm__ volatile ("int3");
    // int a = 1 / 0;

    printk("\n\n");
    printk("Hello FastOS! This is uart... \n");

    printk("Fast OS %x \n", 100);
}

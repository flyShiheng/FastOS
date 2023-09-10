#include "../cpu/idt.h"
#include "../cpu/gdt.h"
#include "../driver/vga_print.h"
#include "../driver/uart.h"

void kernel_main() {

    uart_init();

    gdt_init();

    idt_init();

    printk("\n\n");

    printk("Hello Fast OS%x \n", 100);

    int a = 1 / 0;
    // __asm__ volatile ("int3");
}

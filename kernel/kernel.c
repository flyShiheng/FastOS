#include "../cpu/idt.h"
#include "../cpu/gdt.h"
#include "../driver/vga_print.h"
#include "../driver/uart.h"

void kernel_main() {
    vga_screen_clear();
    vga_print("Kernel Main Start ", TEXT_YELLOW);

    gdt_init();
    idt_init();
    uart_init();

    // __asm__ volatile ("int3");
    // int a = 1 / 0;

    vga_print("Hello FastOS", TEXT_YELLOW);

    printk("\n\n");
    printk("Hello FastOS! This is uart... \n");
}

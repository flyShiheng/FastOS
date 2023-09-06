#include "../interrupt/idt.h"
#include "../interrupt/gdt.h"
#include "../driver/vga_print.h"

void kernel_main() {
	vga_print("Kernel Main Start", TEXT_RED);

	gdt_assemble();
    idt_assemble();

	__asm__ volatile ("int3");
	int a = 1 / 0;
	__asm__ volatile ("cli; hlt");

	vga_print("Hello FastOS", TEXT_YELLOW);
}

[bits 16]
boot_start:
    call print_real_mode_nl
    call set_print_real_mode_cursor
    mov bx, MSG_BOOT_START
    call print_real_string

    call switch_long_mode
    jmp $

%include "boot/boot_print.asm"
%include "boot/boot_gdt.asm"
%include "boot/boot_long.asm"

MSG_BOOT_START db "Boot Start", 0

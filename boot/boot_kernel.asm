[bits 16]
boot_start:
    call print_real_mode_nl
    call set_print_real_mode_cursor
    mov bx, MSG_BOOT_START
    call print_real_string
    jmp $

%include "boot/boot_print.asm"

MSG_BOOT_START db "BOOT START", 0

[org 0x7c00]
kERNEL_OFFSET equ 0x1000
start:
    mov bp, 0x9000
    mov sp, bp
    call set_print_init
    call set_print_real_mode_page
    mov bx, MSG_REAL_MODE
    call print_real_string
    call print_real_mode_nl
    call load_kernel
    call set_print_real_mode_cursor
    mov bx, MSG_LOAD_DISK_SUCCESS
    call print_real_string
    jmp $

[bits 16]
load_kernel:
    pusha
    mov bx, kERNEL_OFFSET     ; 读取的磁盘的数据会保存在bx
    mov dh, 31                ; 读取磁盘的扇区数
    call load_disk
    popa
    ret

%include "boot/boot_print.asm"
%include "boot/boot_disk.asm"

MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_DISK_ERROR db "Read Disk Error in 16-bit Real Mode", 0
MSG_LOAD_DISK_SUCCESS db "Loaded Disk Success", 0

times 510 - ($ - $$) db 0
dw 0xaa55

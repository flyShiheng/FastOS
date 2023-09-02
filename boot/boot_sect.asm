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

print_real_string:
    pusha
print_start:
    mov al, [bx]  ; bx中保存着数据的首地址
    cmp al, 0     ; 为空就返回
    je print_done
    mov ah, 0x0e  ; 显示char字符
    int 0x10
    add bx, 1
    jmp print_start
print_done:
    popa
    ret

print_real_mode_nl:
    pusha
    mov ah, 0x0e
    mov al, 0x0a  ; ASCII 0x0a = 10 = LF 换行
    int 0x10
    popa
    ret

set_print_init:
    pusha
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    popa
    ret

set_print_real_mode_page:
    pusha
    mov ah, 0x02
    mov bh, 0x0
    int 0x10
    popa
    ret

set_print_real_mode_cursor:
    pusha
    mov ah, 0x03
    mov bh, 0x0
    int 0x10

    mov ah, 0x02
    ; mov dh, 0x0  ; 行
    mov dl, 0  ; 列
    int 0x10
    popa
    ret

[bits 16]
load_kernel:
    pusha
    mov bx, kERNEL_OFFSET     ; 读取的磁盘的数据会保存在bx
    mov dh, 31                ; 读取磁盘的扇区数
    call load_disk
    popa
    ret

%include "boot/boot_disk.asm"

MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_DISK_ERROR db "Read Disk Error in 16-bit Real Mode", 0
MSG_LOAD_DISK_SUCCESS db "Loaded Disk Success", 0

times 510 - ($ - $$) db 0
dw 0xaa55

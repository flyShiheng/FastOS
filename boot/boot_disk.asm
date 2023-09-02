[bits 16]
load_disk:
    pusha
    push dx
    mov ah, 0x02
    mov al, dh                ; 读取的扇区的数量
    mov ch, 0
    mov cl, 0x02              ; 从哪个扇区开始
    mov dh, 0
    int 0x13
    jc load_disk_error        ; CF = 0 successful; CF = 1 error
    pop dx
    cmp al, dh
    jne load_disk_sector_error
    popa
    ret

load_disk_error:
    mov bx, MSG_DISK_ERROR
    call print_real_string
    call print_real_mode_nl
    jmp $

load_disk_sector_error:
    mov bx, MSG_DISK_ERROR
    call print_real_string
    call print_real_mode_nl
    jmp $

[bits 16]
switch_protecd_mode:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax
    jmp CODE_SEG:protect_start

[bits 32]
protect_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000 ; 6. update the stack right at the top of the free space
    mov esp, ebp
    mov ebx, MSG_PROT_MODE
    call print_pm
    ; call kernel_main
    jmp $

MSG_PROT_MODE db "Loaded in 32-bit Protected Mode", 0

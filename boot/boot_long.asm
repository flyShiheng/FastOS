%define FREE_SPACE 0xA000
[bits 16]
%define LM_PAGE_PRESENT    (1 << 0)
%define LM_PAGE_WRITE      (1 << 1)

switch_long_mode:
    xor ax, ax
    ; Set up segment registers.
    mov ss, ax
    ; Set up stack so that it starts below Main.
    mov sp, switch_long_mode
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    cld
    mov edi, FREE_SPACE ; Point edi to a free space bracket.
    call switch_long_mode_start

switch_long_mode_start:
    push di
    mov ecx, 0x1000
    xor eax, eax
    cld
    rep stosd
    pop di

    lea eax, [es:di + 0x1000]
    or eax, LM_PAGE_PRESENT | LM_PAGE_WRITE
    mov [es:di], eax

    lea eax, [es:di + 0x2000]
    or eax, LM_PAGE_PRESENT | LM_PAGE_WRITE
    mov [es:di + 0x1000], eax

    lea eax, [es:di + 0x3000]
    or eax, LM_PAGE_PRESENT | LM_PAGE_WRITE
    mov [es:di + 0x2000], eax
    push di

    lea di, [di + 0x3000]
    mov eax, LM_PAGE_PRESENT | LM_PAGE_WRITE
LoopPageTable:
    mov [es:di], eax
    add eax, 0x1000
    add di, 8
    cmp eax, 0x200000
    jb LoopPageTable


    pop di
    ; Disable IRQs
    mov al, 0xFF
    out 0xA1, al
    out 0x21, al
    nop
    nop
    lgdt [GDT_descriptor]                  ; Load GDT.Pointer defined below.
    lidt [IDT_descriptor]                  ; Load a zero length IDT so that any NMI causes a triple fault.

    ; Enter long mode.
    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax

    mov edx, edi                      ; Point CR3 at the PML4.
    mov cr3, edx

    mov ecx, 0xC0000080               ; Read from the EFER MSR. 
    rdmsr    

    or eax, (1 << 8)                ; Set the LME bit.
    wrmsr

    mov ebx, cr0                      ; Activate long mode -
    or ebx,0x80000001                 ; - by enabling paging and protection simultaneously.
    mov cr0, ebx                    

    jmp LONG_CODE_SEG:LongMode


[BITS 64]
[extern kernel_main]
LongMode:
    mov ax, LONG_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Blank out the screen to a blue color.
    mov edi, 0xB8000
    mov rcx, 500                      ; Since we are clearing uint64_t over here, we put the count as Count/4.
    mov rax, 0x1F201F201F201F20       ; Set the value to set the screen to: Blue background, white foreground, blank spaces.
    rep stosq                         ; Clear the entire screen. 
 
    ; ; Display "Hello World!"
    mov edi, 0x00b8000              
 
    mov rax, 0x1F6C1F6C1F651F48    
    mov [edi],rax
 
    mov rax, 0x1F6F1F571F201F6F
    mov [edi + 8], rax

    mov rax, 0x1F211F641F6C1F72
    mov [edi + 16], rax

    call kernel_main

    jmp $

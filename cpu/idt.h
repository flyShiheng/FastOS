#pragma once
#include <stdint.h>

#define IDT_MAX_DESCRIPTORS 			256
#define IDT_CPU_EXCEPTION_COUNT			32

#define IDT_DESCRIPTOR_X16_INTERRUPT	0x06
#define IDT_DESCRIPTOR_X16_TRAP 		0x07
#define IDT_DESCRIPTOR_X32_TASK 		0x05
#define IDT_DESCRIPTOR_X32_INTERRUPT  	0x0E
#define IDT_DESCRIPTOR_X32_TRAP			0x0F
#define IDT_DESCRIPTOR_RING1  			0x40
#define IDT_DESCRIPTOR_RING2  			0x20
#define IDT_DESCRIPTOR_RING3  			0x60
#define IDT_DESCRIPTOR_PRESENT			0x80

#define IDT_DESCRIPTOR_EXCEPTION		(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_EXTERNAL			(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_CALL				(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT | IDT_DESCRIPTOR_RING3)

typedef struct {
	uint16_t	base_low;
	uint16_t	cs;
	uint8_t	    ist;
	uint8_t     attributes;
	uint16_t    base_mid;
	uint32_t    base_high;
	uint32_t    rsv0;
} __attribute__((packed)) idt_desc_t;

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

typedef struct {
    // auto push by cpu  rip cs rflags rsp dss
    struct {
        uint64_t    vector;
        uint64_t    error_code;
        uint64_t    rip;     // 下一条指令地址 
        uint64_t    cs;      // 代码段寄存器
        uint64_t    rflags;  // 标志寄存器 存储与CPU状态相关的标志
        uint64_t    rsp;
        uint64_t    dss;     // 数据段寄存器
    } base_frame;
} isr_frame_t;

void  idt_reload(idtr_t* idtr);
void  idt_set_descriptor(uint8_t vector, uint64_t isr, uint8_t flags, uint8_t ist);
void  idt_init(void);

void isr_handler();

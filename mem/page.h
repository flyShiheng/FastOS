#pragma once
#include <stdint.h>

/**
 * 0 ---- 128M ---- 194M ---- 256M ---- 1G  -----
 *  system      pt       bitmap    kernel    user
*/

/**
 * 采用4级页表进行地址转换
 * 对于一个64位虚拟地址，地址会被分割成5个部分
 * 64-48位是符号位，用来进行符号扩展  16    0xffff000000000000
 * 47-39位是第四级页表（PML4）的索引  9    0xff8000000000
 * 38-30位是第三级页表（PDP）的索引   9    0x7fc0000000
 * 29-21位是第二级页表（PD）的索引    9    0x3fe00000
 * 20-12位是第一级页表（PT）的索引    9    0x1ff000
 * 11-00位是页内偏移                12   0xFFF        
 * 每一级页表都有512个表项，每一个表项大小是8字节
*/

#define PTE_P            0x001                                   // Present
#define PTE_W            0x002                                   // Writable
#define PTE_U            0x004                                   // User

typedef struct {
    uint64_t pml4;
    uint64_t pml3;
    uint64_t pml2;
    uint64_t pml1;
} paging_indexer_t;

typedef struct { 
	uint64_t    entries[512];
} paging_table_t;

static
__attribute__((always_inline)) 
inline void paging_indexer_assign(paging_indexer_t* indexer, void* address);

void mem_init();

uint64_t get_physaddr(void *virtualaddr);

void map_page(void *physaddr, void *virtualaddr, unsigned int flags);

void unmap_page(void *phys, void *virt, unsigned int flags);

uint64_t get_phys_mem_load();

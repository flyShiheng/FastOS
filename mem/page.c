#include "./page.h"
#include "../driver/uart.h"

uint8_t* mem_bitmap = (uint8_t*)0x7ede000;

void paging_indexer_assign(paging_indexer_t* indexer, void* address) {
    uint64_t uaddress = (uint64_t)address;
    uaddress >>= 12;
    indexer->pml1 = uaddress & 0x1ff;
    uaddress >>= 9;
    indexer->pml2 = uaddress & 0x1ff;
    uaddress >>= 9;
    indexer->pml3 = uaddress & 0x1ff;
    uaddress >>= 9;
    indexer->pml4 = uaddress & 0x1ff;
}

// range 32 * 512 * 512 * 4k = 2^5 * 2^9 * 2^9 * 2^12 = 2^35 = 32G
void mem_init2() {
    uint64_t* pml4 = (uint64_t*)0x7e00000;
    uint64_t* pml3 = (uint64_t*)0x7e01000;
    uint64_t* pml2 = (uint64_t*)0x7e02000;
    uint64_t* pml1 = (uint64_t*)0x7e03000;
    uint64_t pml4_phys = get_physaddr((uint64_t)pml4);
    uint64_t pml3_phys = get_physaddr((uint64_t)pml3);
    uint64_t pml2_phys = get_physaddr((uint64_t)pml2);
    uint64_t pml1_phys = get_physaddr((uint64_t)pml1);

    pml4[0] = (uint64_t)pml3_phys | PTE_P | PTE_W;
    pml3[0] = (uint64_t)pml2_phys | PTE_P | PTE_W;
    pml2[0] = (uint64_t)pml1_phys | PTE_P | PTE_W;

    for (uint64_t i = 0; i < 512; i++) {
        uint64_t addr = (0 + i * 0x1000) | PTE_P | PTE_W;
        pml1[i] = addr;
    }
    pml1 = 0x7e03000 + 0x1000 * 63;
    pml1_phys = get_physaddr((uint64_t)pml1);
    pml2[63] = pml1_phys | PTE_P | PTE_W;
    for (uint64_t i = 0; i < 512; i++) {
        pml1[i] = (0x7e00000 + i * 0x1000) | PTE_P | PTE_W;
    }
    // 0x7ee0000 / 0x1000 = 32480  < 4096 * 8 = 32768
    __asm__ volatile("mov %0, %%cr3" : : "a"(pml4_phys));

    for (uint64_t i = 0; i < 512; i++) { 
        set_bitmap(i * 0x1000);
    }
    printk("Mem Init End\n");
}

void mem_init() {
    MemoryMapEntry* mmap = (MemoryMapEntry*)0x8000;
    while ((mmap->type <= 4) && (mmap->type >= 1) && (mmap->length != 0)) {
        printk("MEM Address: %x, Length: %x, type: %d\n", mmap->address, mmap->length, mmap->type);
        mmap++;
    }

    uint64_t* pml4 = 0x101000;  // 1M + 4k
    uint64_t* pml3 = 0x102000;
    uint64_t* pml2 = 0x103000;
    uint64_t* pml1 = 0x104000;
    pml4[0] = (uint64_t)pml3 | PTE_P | PTE_W;
    pml3[0] = (uint64_t)pml2 | PTE_P | PTE_W;
    pml2[0] = (uint64_t)pml1 | PTE_P | PTE_W;
    for (uint64_t i = 0; i < 512; i++) {
        pml1[i] = (0 + i * 0x1000) | PTE_P | PTE_W;
    }
    pml1 = 0x106000;
    pml2[63] = (uint64_t)pml1 | PTE_P | PTE_W;
    for (uint64_t i = 0; i < 100; i++) {
        pml1[i] = (0x7e00000 + i * 0x1000) | PTE_P | PTE_W;
    }
    __asm__ volatile("mov %0, %%cr3" : : "a"(pml4));

    mem_init2();
}

uint64_t get_physaddr(void *virt) {
    paging_indexer_t page_index;
    paging_indexer_assign(&page_index, virt);
    // printk("get_physaddr page_index pml4:%d pml3:%d pml2:%d pml1:%d\n", page_index.pml4, page_index.pml3, page_index.pml2, page_index.pml1);

    uint64_t* pml4 = (uint64_t*)get_phys_mem_load();
    uint64_t* pml3 = (uint64_t*)(pml4[page_index.pml4] & ~0xFFF);
    uint64_t* pml2 = (uint64_t*)(pml3[page_index.pml3] & ~0xFFF);
    uint64_t* pml1 = (uint64_t*)(pml2[page_index.pml2] & ~0xFFF);
    uint64_t  page = pml1[page_index.pml1];
    //printk("get_physaddr addr pml4:%x pml3:%x pml2:%x pml1:%x page:%x\n", pml4, pml3, pml2, pml1, page);
    return ((page & ~0xFFF) | ((uint64_t)virt & 0xFFF));
}

void map_page(void *phys, void *virt, unsigned int flags) {
    paging_indexer_t page_index;
    paging_indexer_assign(&page_index, virt);
    
    return;
}

void unmap_page(void *phys, void *virt, unsigned int flags) {
    return;
}

uint64_t get_phys_mem_load() {
    uint64_t cr3 = 0;
    __asm__ volatile("mov %%cr3, %0" : "=r" (cr3));
    return cr3;
}

void set_bitmap(uint64_t addr) {
    if (addr >= 0x7e00000) {
        return ;
    }
    uint64_t tmp = addr / 0x1000;
    uint64_t index = tmp / 8;
    uint64_t pos = tmp % 8;
    mem_bitmap[index] |= (1 << pos);
}

void set_unbitmap(uint64_t addr) {
    if (addr >= 0x7e00000) {
        return ;
    }
    uint64_t tmp = addr / 0x1000;
    uint64_t index = tmp / 8;
    uint64_t pos = tmp % 8;
    uint8_t bit = 0xFF;
    mem_bitmap[index] &= ((0 << pos) & bit);
}

uint64_t get_bitmap(uint64_t addr) {
    if (addr >= 0x7e00000) {
        return 1;
    }
    uint64_t tmp = addr / 0x1000;
    uint64_t index = tmp / 8;
    uint64_t pos = tmp % 8;
    return ((1 << pos) && mem_bitmap[index]);
}

#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h"

#define PMM_MAX_SIZE 0x20000000

// 4k一页
#define PMM_PAGE_SIZE 0x1000

#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)


// 内核在内存中的位置，在链接器里定义的
extern uint8_t kern_start[];
extern uint8_t kern_end[];

extern uint32_t phy_page_count;

void show_memory_map();

void pmm_init();

void pmm_free(uint32_t p);

uint32_t pmm_alloc_page();


#endif
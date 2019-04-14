#include "multiboot.h"
#include "common.h"
#include "debug.h"
#include "pmm.h"

static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];

static uint32_t pmm_stack_top;

uint32_t phy_page_count;

void show_memory_map() 
{
    uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
    uint32_t mmap_length = glb_mboot_ptr->mmap_length;

    printk("Memory map:\n");
    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
        //printk("mmap addr = %d\n\n", (uint32_t)mmap);
        printk("base_addr = 0x%X%08X, length = 0x%X%08X type = 0x%X\n",
                (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
                (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
                (uint32_t)mmap->type);
    }
}

void pmm_init() 
{
    mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
    mmap_entry_t *mmap_end_adder = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;
    
    mmap_entry_t *map_entry;
    for(map_entry = mmap_start_addr; map_entry < mmap_end_adder; map_entry++) {
        //内存是可用的而且要从1M以后开始
        if(map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {
            // 要从内核后面开始
            uint32_t page_addr = map_entry->base_addr_low + (uint32_t)(kern_end - kern_start);
            while (page_addr < map_entry->base_addr_low + map_entry->length_low && page_addr <= PMM_MAX_SIZE) {
                // 分页然后放到内存栈里
                //pmm_stack[++pmm_stack_top] = page_addr;
                pmm_free(page_addr);
                page_addr += PMM_PAGE_SIZE;
                phy_page_count++;
            }
            
        }
    }
}

void pmm_free(uint32_t p)
{
    // 指针为PAGE_MAX_SIZE时说明爆栈了(因为是++pmm_stack_top,倒这里说明已经满了，不能继续如栈了)
    assert(pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack!");
    pmm_stack[++pmm_stack_top] = p;
}

uint32_t pmm_alloc_page() 
{
    assert(pmm_stack_top != 0, "out of memory");
    //返回分配的页地址
    return pmm_stack[--pmm_stack_top];
}
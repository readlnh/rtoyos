#include "debug.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

static void alloc_chunk(uint32_t start, uint32_t len);

static void free_chunk(header_t *chunk);

static void split_chunk(header_t *chunk, uint32_t len);

static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

// 头指针
static header_t *heap_first;

void init_heap() 
{
    heap_first = 0;
}

void *kmalloc(uint32_t len)
{
    len = len + sizeof(header_t);

    header_t *cur_header = heap_first;
    header_t *prev_header = 0;

    while (cur_header) {
        if(cur_header->allocated == 0 && cur_header->length >= len) {
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            // 返回时指针应该在管理结构之后
            return (void *)((uint32_t)cur_header + sizeof(header_t));
        }
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    uint32_t chunk_start;

    if(prev_header) {
        chunk_start = (uint32_t)prev_header + prev_header->length;
    } else {
         chunk_start = HEAP_START;
         heap_first = (header_t *)chunk_start;
    }

    alloc_chunk(chunk_start, len);
    cur_header = (header_t *)chunk_start;
    cur_header->prev = prev_header;
    cur_header->next = 0;
    cur_header->length = len;
    cur_header->allocated = 1;

    if(prev_header) {
        prev_header->next = cur_header;
    }

    return (void*)((uint32_t)cur_header + sizeof(header_t));   
}

void kfree(void *p)
{
    header_t *header = (header_t *)((uint32_t)p - sizeof(header_t));
    header->allocated = 0;

    glue_chunk(header);
}

void alloc_chunk(uint32_t start, uint32_t len) 
{
    // 循环申请，直到内存够了为止
    while (start + len > heap_max) {
        uint32_t page = pmm_alloc_page();
        map(pgd_kern, heap_max, page, PAGE_PRESENT | PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

void free_chunk(header_t *chunk)
{
    if(chunk->prev == 0) {
        heap_first = 0;
    } else {
        chunk->prev->next = 0;
    }

    // 如果空闲超过1页就释放掉
    while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk) {
        heap_max -= PAGE_SIZE;
        uint32_t page;
        get_mapping(pgd_kern, heap_max, &page);
        unmap(pgd_kern, heap_max);
        pmm_free(page);
    }
    
    
}

void split_chunk(header_t *chunk, uint32_t len)  
{
    // 切分后剩余内存至少要能容纳一个管理块
    // 切分即将原chunk长度变为len，剩下的就是newchunk
    if(chunk->length - len > sizeof(header_t)) {
        header_t *newchunk = (header_t *)((uint32_t)chunk + chunk->length);
        newchunk->prev = chunk;
        newchunk->next = chunk->next;
        newchunk->allocated = 0;
        newchunk->length = chunk->length - len;

        chunk->length = len;
        chunk->next = newchunk;
    }
}

void glue_chunk(header_t *chunk)
{
    // 如果后面的内存块没在使用
    if(chunk->next && chunk->next->allocated == 0) {
        chunk->length = chunk->length + chunk->next->length;
        if(chunk->next->next) {
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }

    // 如果前面的内存块没在用
    if(chunk->prev && chunk->prev->allocated == 0) {
        chunk->prev->length = chunk->prev->length + chunk->length;
        chunk->prev->next = chunk->next;
        if(chunk->next) {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }
    
    if(chunk->next == 0) {
        free_chunk(chunk);
    }
}

void test_heap()
{
    printk_color(rc_black, rc_red, "Test kmalloc && kfree..\n");
    
    void *addr1 = kmalloc(50);
    printk("kmalloc 50 byte in 0x%X\n", addr1);
    void *addr2 = kmalloc(500);
    printk("kmalloc 500 byte in 0x%X\n", addr2);

    printk("free memory in 0x%X\n", addr2);
    kfree(addr2);

    void *addr3 = kmalloc(50);
    printk("kmalloc 50 byte in 0x%X\n", addr3);

     void *addr4 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%X\n", addr4);
}
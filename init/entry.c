#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"

#include "vmm.h"

// 内核初始化函数
void kernel_init();

// 开启分页后的Multiboot数据指针
multiboot_t *glb_mboot_ptr;

// 开启分页后的内核栈
char kern_stack[STACK_SIZE];

__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;


__attribute__((section(".init.text"))) void kernel_entry()
{
    pgd_tmp[0] = (uint32_t) pte_low | PAGE_PRESENT | PAGE_WRITE;
    pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t) pte_high | PAGE_PRESENT | PAGE_WRITE;

    int i;
    // 映射0-4M
    for(i = 0; i < 1024; i++) {
        pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    // 映射0x00000000-0x00400000到0xC0000000-0xC0400000
    for(i = 0; i < 1024; i++) {
        pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    asm volatile("mov %0, %%cr3" : : "r" (pgd_tmp));
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r" (cr0));
    // 将cr0置为1就会启用分页
    cr0 = cr0 | 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r" (cr0));


    uint32_t kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xfffffff0;
    //**切换内核栈**
    asm volatile("mov %0, %%esp\n\t"
                    "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

    glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;


    kernel_init();
}



void kernel_init() 
{
    init_debug();
    init_gdt();
    init_idt();
    
    console_clear();
    printk_color(rc_black, rc_cyan, "Hello world\n");

    
    //init_test();
    
    //asm volatile("int $0xff");

    init_timer(200);

    //asm volatile("sti");

    printk("kernel in memory start: 0x%08X\n", kern_start);
    printk("kernel in memory end: 0x%08X\n", kern_end);
    printk("kernel in memory used: %dKB\n", (kern_end - kern_start + 1023) / 1024);

    //show_memory_map();
    
    pmm_init();
    init_vmm();
    
    printk("\nThe count of Physicak Memory Page is: %d\n\n", phy_page_count);

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());
    
    
    while (1)
    {
        asm volatile("hlt");
    }
    
    //return 0;
}
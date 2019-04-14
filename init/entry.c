#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"

int kernel_entry() 
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

    show_memory_map();

    pmm_init();

    printk("\nThe count of Physicak Memory Page is: %d\n\n", phy_page_count);

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());

    printk("Test pmm_alloc_page\n");
    printk("Alloc Physical Addr: 0x%08X\n", pmm_alloc_page());
    
    return 0;
}
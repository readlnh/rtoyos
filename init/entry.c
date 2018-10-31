#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"

int kernel_entry() 
{
    init_debug();
    init_gdt();
    init_idt();
    
    console_clear();
    printk_color(rc_black, rc_cyan, "Hello world\n");

    //register_interrupt_handler(255, *p);
    
    asm volatile("int $0x3");
    asm volatile("int $0x4");
    
    return 0;
}
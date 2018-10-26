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

    asm volatile("int $0x3");
    
    return 0;
}
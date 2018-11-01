#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"

int kernel_entry() 
{
    init_debug();
    init_gdt();
    init_idt();
    
    console_clear();
    printk_color(rc_black, rc_cyan, "Hello world\n");

    
    //init_test();
    
    asm volatile("int $0xff");

    init_timer(200);

    asm volatile("sti");
    
    return 0;
}
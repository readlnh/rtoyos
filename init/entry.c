#include "console.h"
#include "debug.h"
#include "gdt.h"

int kernel_entry() 
{
    init_gdt();
    console_clear();
    //console_write_color("Hello readlnh\n", rc_blue, rc_red);
    int x = 10;
    printk("%d\n", x);
    return 0;
}
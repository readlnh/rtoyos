#include "console.h"
#include "debug.h"
#include "gdt.h"

int kernel_entry() 
{
    init_gdt();
    init_debug();
    console_clear();
    readlnh_printf("%s\n", "Hi");
    int x = 10;
    printk("%d\n", x);
    return 0;
}
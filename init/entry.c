#include "console.h"
#include "debug.h"

int kernel_entry() 
{
    console_clear();
    //console_write_color("Hello readlnh\n", rc_blue, rc_red);
    int x = 10;
    printk("%d\n", x);
    return 0;
}
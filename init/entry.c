#include "console.h"

int kernel_entry() 
{
    console_clear();
    console_write_color("Hello readlnh\n", rc_blue, rc_red);
    return 0;
}
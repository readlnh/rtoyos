#include "timer.h"
#include "debug.h"
#include "common.h"
#include "idt.h"

void timer_callback(pt_regs *regs)
{
    static uint32_t tick = 0;
    printk_color(rc_light_blue, rc_red, "Tick: %d\n", tick++);
}

/*
 * frequency是中断时钟的频率
*/
void init_timer(uint32_t frequency)
{
    register_interrupt_handler(IRQ0, timer_callback);

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    uint8_t low = (uint8_t)(divisor & 0xff);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xff);

    outb(0x40, low);
    outb(0x40, high);
}

/*void test_callback(pt_regs *regs)
{
    printk_color(rc_light_blue, rc_red, "Ttttest\n");
}


void init_test()
{
    register_interrupt_handler(255, test_callback);
    
}*/


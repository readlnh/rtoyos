#include "common.h"
#include "string.h"
#include "debug.h"
#include "idt.h"

//中断描述符表
idt_entry_t idt_entries[256];

//IDTR
idt_ptr_t idt_ptr;

//中断处理函数的指针数组
interrupt_handler_t interrupt_handlers[256];

//设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

//声明加载IDTR的函数
extern void idt_flush(uint32_t);

void init_idt()
{
    /*
	 * 重新映射IRQ表
	 * 两片级联的Intel 8259A
	 * 主片端口 0x20 0x21
	 * 从片端口 0xA0 0xA1
	 * A0线用于选择操作的寄存器。在PC/AT微机系统中，
	 * 当A0=0时芯片的端口地址是0x20(主芯片)和0xA0(从芯片）；
	 * 当 A0=1时端口就是0x21(主芯片)和0xA1(从芯片)。
	*/

	/* 
	 * 初始化主从片ICW1 
	 * d0: 1-需要ICW4 0-不需要
	 * d4: 恒为1
	 * 0001 0001 
	*/
	outb(0x20, 0x11);
	outb(0xa0, 0x11);

	/*
	 * ICW2
	 * 设置了ICW之后，当A0=1时表示对ICW2进行设置
	 * 主端口0x21 从端口0xa1
	 * d7~d3为中断类型的高5位,d2~d0为任意值，一般为000
	 * 0x20(32):0x20~0x27 0x28(40):0x28~0x2f
	 * 主片中断号从32开始,从片从40开始
	*/
	outb(0x21, 0x20);
	outb(0xa1, 0x28);

	/*
	 * ICW3
	 * 设置主片 IR2 引脚连接从片
	 * 告诉从片输出引脚和主片 IR2 号相连
	*/
	outb(0x21, 0x04);
	outb(0xa1, 0x02);

	/*
	 * ICW4
	 * 普通全嵌套、非缓冲、非自动结束中断方式
	*/
	outb(0x21, 0x01);
	outb(0xa1, 0x01);

	// 设置主从片允许中断
	outb(0x21, 0x0);
	outb(0xa1, 0x0);
	
	bzero((uint8_t *)&interrupt_handlers, sizeof(interrupt_handler_t) * 256);

    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

	bzero((uint8_t *)&idt_entries, sizeof(idt_entry_t) * 256);

    //0-32: 用于cpu的中断处理
    idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);


	// 255 将来用于实现系统调用
	idt_set_gate(255, (uint32_t)isr255, 0x08, 0x8E);
    
    // 更新设置中断描述符表
    idt_flush((uint32_t)&idt_ptr);
}

//设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xffff;
    idt_entries[num].base_hi = (base >> 16) & 0xffff;

    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;

    //0x60是个魔数，以后实现用户态的时候用
    idt_entries[num].flags = flags;  // | 0x60;
}


// 调用中断处理函数
void isr_handler(pt_regs *regs)
{
    if(interrupt_handlers[regs->int_no]) {
		/*
		 * 具体的中断处理函数的原型都是void(pt_regs*),
		 * 它们被统一组织放置在了全局的函数指针数组interrupt_handers里面。
		 * idt_hanlder函数如判断这个中断函数是否注册,
		 * 如果注册了会执行该函数,
		 * 否则打印出Unhandled interrupt和中断号码。
		*/
        interrupt_handlers[regs->int_no](regs);
    } else {
        printk_color(rc_white, rc_black, "Unhandled interrupt: %d\n", regs->int_no);
    }
}

// 注册一个中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h)
{
	interrupt_handlers[n] = h;
}

// IRQ处理函数
void irq_handler(pt_regs *regs)
{
	// 发送中断结束信号给 PICs
	// 按照我们的设置，从 32 号中断起为用户自定义中断
	// 因为单片的 Intel 8259A 芯片只能处理 8 级中断
	// 故大于等于 40 的中断号是由从片处理的
	if(regs->int_no >= 40) {
		// 发送重设信号给从片
		outb(0xa0, 0x20);
	}
	// 发送重设信号给主片
	outb(0x20, 0x20);
	
	if(interrupt_handlers[regs->int_no]) {
		interrupt_handlers[regs->int_no](regs);
	}
}
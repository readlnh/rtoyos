#include "common.h"
#include "string.h"
#include "debug.h"
#include "idt.h"

//中断描述符表
idt_entry_t idt_entries[256];

//IDTR
idt_ptr_t idt_ptr;

//终端处理函数的指针数组
interrupt_handler_t interrupt_handlers[256];


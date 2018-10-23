#ifndef INCLUDE_IDT_H_
#define INCLUDE_IDT_H_

#include "types.h"

// 初始化中断描述符表
void init_idt();

// 初始化中断描述符表
typedef
struct idt_entry_t {
    uint16_t base_lo;   // 中断处理函数地址15:0
    uint16_t sel;       // 目标代码段描述符选择子
    uint8_t always0;    // 置0段
    uint8_t flags;      // 一些标志
    uint16_t base_hi;   // 中断处理函数地址31:16
} __attribute__((packed)) idt_entry_t;

// IDTR
typedef
struct idt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;


#endif
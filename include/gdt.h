#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_

#include "types.h"

/*
|Base 31:32 | G |D/B|L|AVL|SegLimit19:16|P|DPL|S|Type|Base 23:16|
|Base Address15:00|Segment Limit 15:00|
- L -- 64-bit code segment(IA-32e mode only)
- AVL -- Avaliable for use by system software
- Base -- Segment base address
- D/B -- Default operation size (0 = 16-bit segment;1 = 32-bit segment)
- DPL -- Descriptor privilege level 描述符特权级
- G -- Granularity
- Limit -- Segment Limit
- P -- Segment present 段存在位
- S -- Descriptor type(0 = system; 1 = code or data) 描述符子类别
- Type -- Segment type 描述符类型
*/

//全局描述符
typedef
struct gdt_entry_t {
    uint16_t limit_low;     //段界限15:00
    uint16_t base_low;      //段基址15:0
    uint8_t  base_middle;   //段基址23:16
    uint8_t  access;        //段存在位、描述符特权级、描述符类型、描述符子类别
    uint8_t  granularity;   //其他标志、段界限19:16
    uint8_t  base_high;     //段基址31:24

} __attribute__((packed)) gdt_entry_t;

//GDTR
typedef
struct gdt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

void init_gdt();

// GDT加载到GDTR的函数(汇编实现[])
extern void gdt_flush(uint32_t);


#endif
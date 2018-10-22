#include "gdt.h"
#include "string.h"

#define GDT_LENGTH 5

// GDT表的长度
gdt_entry_t gdt_entries[GDT_LENGTH];

//GDTR
gdt_ptr_t gdt_ptr;

static void gdt_set_gate(int32_t num, uint32_t base,
                        uint32_t limit, uint8_t access, uint8_t gran);

// 声明内核栈
extern uint32_t stack;

void init_gdt() 
{
    //界限从0开始，故需要减一
    gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_LENGTH - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    //Intel平坦模型
    gdt_set_gate(0, 0, 0, 0, 0); //第一个描述符必须全为0
    gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf); //指令段
    gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf); //数据段  
    gdt_set_gate(3, 0, 0xffffffff, 0xfa, 0xcf); //用户模式代码段
    gdt_set_gate(4, 0, 0xffffffff, 0xf2, 0xcf); //用户模式数据段

    //加载全局描述符表地址到gptr寄存器
    gdt_flush((uint32_t)&gdt_ptr);
}

//分别为数组下标，基地址，界限，访问标志，其他访问标志
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t acess, uint8_t gran)
{
    gdt_entries[num].base_low = (base & 0xffff);
    gdt_entries[num].base_middle = (base >> 16) & 0xff;
    gdt_entries[num].base_high = (base >> 24) & 0xff;

    gdt_entries[num].limit_low = limit & 0xffff;
    gdt_entries[num].granularity = (limit >> 16) & 0x0f;

    /*
     * | G |D/B|L|AVL|SegLimit19:16|
    */
    gdt_entries[num].granularity |= gran & 0xf0;

    /* 
     * |P|DPL|S|Type| 
     * P位称为存在(Present)位,
     * P=1表示描述符对地址转换是有效的，或者说该描述符所描述的段存在，即在内存中；
     * P=0表示描述符对地址转换无效，即该段不存在。使用该描述符进行内存访问时会引起异常。
     * 
     * DPL表示描述符特权级(Descriptor Privilege level)
     * 共2位。它规定了所描述段的特权级，用于特权检查，以决定对该段能否访问。
     * 
     * S位(DT位) Descriptor type说明描述符的类型(0 = system; 1 = code or data)
     * 该位为 1 表示这是一个数据段或者代码段。为 0 表示这是一个系统段（比如调用门，中断门等） 
     * 
     * TYPE 占4位 具体可参见gdt.md
    */ 
    gdt_entries[num].access = acess;
}
#ifndef INCLUDE_VMM_H
#define INCLUDE_VMM_H

#include "types.h"
#include "idt.h"

// 内核的偏移地址
#define PAGE_OFFSET 0xC0000000

/**
* P−− 位 0 是存在 (Present) 标志,用于指明表项对地址转换是否有效。
* P = 1 表示有效; P = 0 表示无效。
* 在页转换过程中,如果说涉及的页目录或页表的表项无效,则会导致一个异常。
* 如果 P = 0 ,那么除表示表项无效外,其余位可供程序自由使用。
* 例如,操作系统可以使用这些位来保存已存储在磁盘上的页面的序号。
*/
#define PAGE_PRESENT 0x1

/**
* R/W −− 位 1 是读 / 写 (Read/Write) 标志。如果等于 1 ,表示页面可以被读、写或执行。
* 如果为 0 ,表示页面只读或可执行。
* 当处理器运行在超级用户特权级(级别 0,1 或) 2 时,则 R/W 位不起作用。
* 页目录项中的 R/W 位对其所映射的所有页面起作用。
*/
#define PAGE_WRITE 0x2


/**
* U/S −− 位 2 是用户 / 超级用户 (User/Supervisor) 标志。
* 如果为 1 ,那么运行在任何特权级上的程序都可以访问该页面。
* 如果为 0 ,那么页面只能被运行在超级用户特权级 (0,1 或 2) 上的程序访问。
* 页目录项中的 U/S 位对其所映射的所有页面起作用。
*/
#define PAGE_USER 0x4

// 虚拟页大小
#define PAGE_SIZE 4096

// 页掩码，用于4K对齐
#define PAGE_MASK 0xfffff000

#define PGD_INDEX(x) (((x) >> 22) & 0x3ff)

#define PTE_INDEX(x) (((x) >> 12) & 0x3ff)

#define OFFSET_INDEX(x) ((x) & 0xfff)

// 页目录类型 Page Global Directory
typedef uint32_t pgd_t;

// 页表类型 Page Table
typedef uint32_t pte_t;

// 页目录成员数
#define PGD_SIZE (PAGE_SIZE / sizeof(pte_t))

// 页表成员数
#define PTE_SIZE (PAGE_SIZE/ sizeof(uint32_t))

// 512M内存需要的页表数 (512 * 1024) / (4 * 1024)
#define PTE_COUNT 128

// 内核页目录
extern pgd_t pgd_kern[PGD_SIZE];

void init_vmm();

// 更换当前页目录
void switch_pgd(uint32_t pd);

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

void unmap(pgd_t *pgd_now, uint32_t va);

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

void page_fault(pt_regs *regs);

#endif
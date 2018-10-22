[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4] ;参数存入eax寄存器
    lgdt [eax]         ;加载到GDTR修改原先[设置grub] 

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush    ;0x08 |段描述符index|TI|RPL| index=0 TI=0为空选择子 indx=0 TI=0为第零个选择子

.flush:
    ret
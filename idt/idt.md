# idt的一些记录

## 8259a

### ICW3
主芯片的端口地址是0x21，从芯片的端口地址是0xA1。

对于主片，Si=1,表示IRi接从片的INT引脚。说得啰嗦点，就是主片 S7~S0 各比特位对应级联的从片。哪位为1则表示主片的该中断请求引脚IR上信号来自从片， 否则对应的IR引脚没有连从片。

对于从片，ID2~ID0 三个比特位对应各从片的标识号，即连接到主片的中断级。当某个从片接收到级联线（CAS2—CAS0）输入的值与自己的 ID2~ID0 相等时，表示此从片被选中。此时该从片应该向数据总线发送自己当前被选中的中断请求的中断号。

Linux-0.11 内核把8259A主片的 ICW3 设置为 0x04，即 S2=1，其余各位为0。表示主芯片的 IR2 引脚连接一个从芯片。从芯片的 ICW3 被设置为 0x02，即其标识号为2。表示此从片连接到主片的IR2引脚。 因此，中断优先级的排列次序为：0级最高，1级次之，接下来是从片上的 8~15 级，最后是主片的 3~7 级。 

### ICW4
Linux-0.11内核送往8259A主芯片和从芯片的 ICW4 命令字的值均为 0x01。表示 8259A 芯片被设置成普通全嵌套、非缓冲、非自动结束中断方式，并且用于 8086 及其兼容系统。


## 自定义中断测试
尝试了下自定出255号中断
代码如下
```c
void test_callback(pt_regs *regs)
{
    printk_color(rc_light_blue, rc_red, "Ttttest\n");
}


void init_test()
{
    register_interrupt_handler(255, test_callback);
    
}
```
在入口处初始化255中断，并调用int 255
```c
int kernel_entry() 
{
    init_debug();
    init_gdt();
    init_idt();
    
    console_clear();
    printk_color(rc_black, rc_cyan, "Hello world\n");

    //初始化255中断
    init_test();
    // 进行中断255
    asm volatile("int $0xff");
    
    return 0;
}
```

输出了Ttttest，测试成功
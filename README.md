# proteus-CPU
we final big homework
# ROM 2764
是一种存储芯片，用作存储数据
A0到A12为13条地址信号输入线，说明芯片容量为2的13次方，即8K
D0到D7为数据线，表示芯片的每个存储单元存放一个字节（8位二进制数）。对芯片读数时，作为输出线，对芯片编程时，作为输入线。
CE为输入信号，低电平有效。（有称作片选信号）
OE为输出允许信号，低电平有效
PGM为编程脉冲输入端，当对芯片编程时，由此端加入编程脉冲信号；读取数据时PMG的值为1
Vcc和Vpp都是接电源的，正常工作时是+5V

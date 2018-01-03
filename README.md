# 任务分配

内容 | 状态 | 分配
---  | --- | ---
ALU | 已完成 | CL封装
RAM/ROM | 已完成 | CL
OUT | 简单 | LSY
REGISTER | 简单 | LSY
PC | 简单 | DY
PSW | 未知 | DY
CLOCK | 忽略 | NONE
CONTROLLER | 困难 | ALL

![附图](http://owvqyk7j0.bkt.clouddn.com/proteus-cpu-all.png)


# proteus-CPU

计组大作业

__软件版本__：Proteus 8.4 SP0

__总线__：16位数据，16位地址

__结构__：待定

__寄存器__：四个通用寄存器，其中包括累加寄存器AC

__指令集__：

_必需_：

序号 | 指令 | 功能 | 备注
-----|-----|-----|----
   1  |STO (XXXX), R\_i |  R\_i -> 存储器  | 
   2  |LAD R\_i, (XXXX) |  存储器 -> R\_i  | 
   3  |ADD R\_i |  AC+R\_i -> AC  | 
   4  |SUB R\_i |  AC-R\_i -> AC  | 
   5  |JMP XXXX |  PC <- XXXXH  |  64KB
   6  |CLA |  AC <- 0000H  | 

_可选_：

序号 | 指令 | 功能 | 备注
-----|-----|-----|----
  7  |JC XX | CF=1, PC <- PC+XX  |  XX: -128 ~ 127
  8  |IRET |  中断返回  | 恢复现场
  9  |PUSH R\_i | SP <- SP-1, (SP) <- R\_i  |  写一个机器字
  10  |POP R\_i | R\_i <- (SP), SP <- SP+1  | 


# 工作流

先一起设计好每个元件的接口，

分开做每个元件，放在单独的文件夹里并测试，总目录/component

测试无误后编译并生成元件，放在/build下

最后一起合并


# 元件

## ALU

### 输入

ALU_BUS 低有效，ALU运算结果到BUS的出口开关

ALU_M, ALU_CN, ALU_S0, ALU_S1... 运算控制

DA_CLK, DB_CLK 暂存器A,B的时钟型号,收到信号即打入数据

16位数据到暂存器DA，DB

### 输出

16位运算结果到BUS

## 存储器

### 输入

MEM_OE 低有效 存储器的输出有效信号

R_WE 低有效 存储器写允许信号

LDAR 同步 低有效 AR置数信号

LDIR 同步 低有效 IR置数信号

RAM_WE 同步 低有效 存储器写允许信号

### 输出

16位数据到BUS


## REG_16

自制元件，16为寄存器

输入：

* BUS[0..15]
* CLK XX沿
* EN 高有效
* CLR 高有效

输出：

* OBUS[0..15]




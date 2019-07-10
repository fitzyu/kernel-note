# 中断
中断是什么 中断时CPU对系统发生某个事件做出的一种反应
CPU暂停执行的程序，保留现场后自动去执行相应事件的处理程序 处理完成后返回断点继续执行被打断的程序

中断时操作系统的脉搏，是并发的基础
中断的引入是为了支持CPU和设备之间的并行操作

  CPU收到中断信号以后，并不立即响应中断，而是在执行每条指令周期的最后一个时钟周期，一旦检测到中断信号有效，并且中断允许标志位置1的时候，cpu才在当前指令执行完以后转入中断响应周期


中断模型从简单的来讲实际上是一种C/S结构

外设发出请求，并不是马上传给CPU而是用中断控制器进行收集，中断控制器就相当于中介，在外设与CPU之间架起了桥梁，当CPU接收到请求之后才给予应答

以8259控制器为例子 中断请求从IR0-IR7从外设发出，探后通过中断控制器的INT引脚向CPU发出中断请求，然后CPU通过中断引脚INTA应答请求

目前x86采用的是APIC 也就是高级可编程控制器

介绍高级可编程中断控制器 每个x86的核都有一个本地的APIC,本地APIC通过中断控制器通信总线连接到IO APIC上，IO APIC收集各个外设的中断，并翻译成总线上的信息，传递给某个CPU上的本地APIC

中断中机制与策略分离的设计理念

CPU的设计独立于中断控制器的设计

！！参考Linux内核之旅的中断趣味谈

Linux内核中的中断子系统的相关软件架构
中断子系统分成4部分
1. 硬件无关的代码 Linux内核通用中断处理模块
2. CPU体系结构相关的中断处理
3. 中断控制器的驱动代码
4. 普通外设的驱动 这些驱动将使用Linux内核通用的中断处理模块的API来实现自己的驱动逻辑

中断向量
中断向量--每个中断源都被分配一个9位无符号整数作为类型码，即中断向量

中断向量和中断请求号是什么关系？ 线性关系

中断描述符表
即中断向量表，每个中断占据一个表项

中断向量表也叫做中断描述符表IDT，其中每一个表项叫做一个门描述符
“门”的含义是当中断发生时必须先通过这些门，然后才能进入相应的处理程序
中断门 陷阱门 系统门

！！动手实践
Linux内核之旅网站 电子杂志栏目第8期 “中断
深入理解Linux内核 第三版第四章
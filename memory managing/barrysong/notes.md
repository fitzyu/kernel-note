#内存管理

cpu是调度器 调度算法
要想学习IO 学习通过学习内存去看

cpu 内存空间 io空间 内存
CPU寻址内存，虚拟地址，物理地址
页表的基地址会存储在MMU的寄存器中

指针是CPU发出访问内存的
物理地址是一个整数
MMU及RWX权限、kernel和user模式权限

内存的zone:DMA、normal和HIGHMEM

linux内存管理的Buddy算法

连续内存分配器(CMA)

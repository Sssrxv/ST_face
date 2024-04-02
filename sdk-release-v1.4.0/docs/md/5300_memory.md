# AI31xx内存映射以及flash分区

* `1.` [AI31xx内存映射](#AI31xx内存映射)
  * `1.1.` [启动流程](#启动流程)
* `2.` [Flash分区介绍](#FlashFlash分区介绍)
  * `2.1.` [固定分区](#固定分区)
  * `2.2.` [自定义分区](#自定义分区)
* `3.` [如何为工厂产品生成闪存ROM映像文件？](#如何为工厂产品生成闪存ROM映像文件)
* `4.` [如何动态更新分区表数据？](#如何动态更新分区表数据)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->


##  1. <a name='AI31xx内存映射'></a>AI31xx内存映射
AI31xx的内存分布如下所示：

- ROM: SOC片内用于存储启动系统的boot程序，只读属性
- SRAM0：SOC片内第一块SRAM
- SRAM2：SOC片内第三块SRAM
- SRAM1：SOC片内第二块SRAM
- HW registers：SOC外设地址分布区域
- DDR3：DDR内存区域


###  1.1. <a name='启动流程'></a>启动流程

1. `BOOT ROM中的程序` 加载 `SPL(二级启动程序)` 到 SRAM0 (0x10000000)
2. `SPL` 加载FW固件到DDR3 (0x80000000)
3. 在DDR3中运行FW固件程序

```c
0x00000000 -------> +-------------------------------------+
                    |     BOOT ROM      (8KB,   RO)       |
0x00002000 -------> +-------------------------------------+
                    .     (*)           (256M - 8KB)      .
0x10000000 -------> +-------------------------------------+
                    |     SRAM0         (256KB, RW)       |
0x10040000 -------> +-------------------------------------+
                    |     SRAM1         (256KB, RW)       |
0x10080000 -------> +-------------------------------------+
                    |     SRAM2         (256KB, RW)       |
0x100C0000 -------> +-------------------------------------+
                    .     (*)           (256M - 768KB)    .
0x20000000 -------> +-------------------------------------+
                    |     HW registers  (4MB, RO/WO/RW)   |
0x20400000 -------> +-------------------------------------+
                    .     (*)           (2GB- 4MB)        .
0x80000000 -------> +-------------------------------------+
                    |     DDR3          (128MB)           |
0x88000000 -------> +-------------------------------------+
                    .     (*)           (*)               .
0xFFFFFFFF -------> +-------------------------------------+
```

##  2. <a name='Flash分区介绍'></a>Flash分区介绍

AI31xxx系列的SDK将flash按功能划分某些区域. 有些区域是固定的,有些区域留给用用户自定义。

###  2.1. <a name='固定分区'></a>固定分区
  - `SPL (二级启动程序)`. SPL分区的启动地址是固定的0x0. boot rom中的程序会自动从flash的起始地址加载程序，也就是启动SPL； SPL程序的大小为固定的256KB；
  - `分区表`.  如下表所示，划分了两个分区Part0和Part1，每一个区域都预留出一部分；如下的分区表详细展示了自定义分区内存分布；

```c
0x00000000 -------> +-------------------------------------+
                    |     SPL           (256KB)           |
0x00040000 -------> +-------------------------------------+
                    |     Part Table0   (1KB)             |
                    +-------------------------------------+
                    .     (*)           (127KB)           .
0x00060000 -------> +-------------------------------------+
                    |     Part Table1   (1KB)             |
                    +-------------------------------------+
                    .     (*)           (127KB)           .
                    +-------------------------------------+
```

###  2.2. <a name='自定义分区'></a>自定义分区

通常，自定义分区的命名和大小用户可以按照要求进行自定义，但是像固定分区的命名不可以修改，例如 ：'fw0', 'fw1' and 'fs'.

  - `fw0 (FW image0)`.  启动时SPL会在分区表中找名字为`fw0`的分区，如果找到了并且验证成功，SPL会乞启动该`fw0`固件；`fw0`分区的大小用户可以通过配置文件改变；
  - `fw1 (FW image1)`.  `fw1`作为`fw0`备份存在；如果`fw0`启动失败将会尝试从`fw1`启动； `fw1`分区的大小用户也可以通过配置文件改变；
  - `fs (File System)`. 如果默认使用AI31xx的SDK，SDK中的程序将会查找分区名为`fs`的分区，如果找到了会挂载`fs`； 如果没有找到`fs`分区，挂在操作会报告挂载失败；`fs`分区的大小可以通过配置文件来定义；
  - 对于其他的分区，分区大小和分区名称用户都可以按需自定义；

```c
0x00080000 -------> +-------------------------------------+
                    |     fw0(FW image0)(xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     fw1(FW image1)(xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     fs(File system)(xxMB)           |
0xXXX00000 -------> +-------------------------------------+
                    |     cust_part0    (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     cust_part1    (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     (...)         (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
```

> 注意：对于用户自定的分区，在内存地上上不应该有重叠；用户需要仔细各分区的起始地址以及偏移地址；

##  3. <a name='如何为工厂产品生成闪存ROM映像文件'></a>如何为工厂产品生成闪存ROM映像文件？
为了构建工厂的flash镜像文件，需要如下6个条件：

1. SPL image
2. FW images
3. ROM configuration file
4. swapdw tool
5. bin2rom tool
6. Linux OS PC

对于`SPL`镜像文件存在于SDK，如果在SDK中没有找到该文件，可联系`FAE`获取该文件。 

固件文件时命名为`fw0`和`fw1`的镜像文件，需要使用`swapdw`工具从目标镜像文件产生，一般目标镜像文件后缀为'.img'。

> 注意：SPL有两种类型的文件，对于NOR flash类型的flash，请使用`nor_spl.img`，对于NAND flash，请使用`nand_spl.img`

如下作为ROM配置文件的一个例子，以'#'起始的一行，该行将会作为注释行；一行的第一个字段为分区的名称，第二个字段为分区的偏移地址，第三个字段通常为分区的大小；

```sh
# `flash_part.cfg` file

# partition name, offset, size
# 1st part: fw region 0 (default)
fw0         0x00080000  0x00100000
# 2nd part: fw region 1 (backup)
fw1         0x00180000  0x00100000
# 3rd part: file system
fs          0x00280000  0x00100000
# 4th part: customized partition
cust0       0x00280000  0x00100000
```

生成`fw0`和`fw1`文件
- 假设工生成的镜像文件名为`a.img`，执行下面命令将会产生`fw0`和`fw1`文件

```sh
$ ./swapdw a.img fw0
$ ./swapdw a.img fw1
```

假设现在拷贝如下所有文件到名为`ROM`文件中，如下列举所示：
```sh
- ROM
    |- nor_spl.img  # if using NAND flash, replace it with 'nand_spl.img'
    |- fw0
    |- fw1
    |- flash_part.cfg
    |- bin2rom
```

在文件夹`ROM`执行如下命令，将产生ROM文件：

假定使用的flash类型为NOR flash:

```sh
$ ./bin2rom nor_spl.img flash_part.cfg 0 nor_flash.rom
```

假定使用的flash类型为NAND flash:
```sh
$ ./bin2rom nand_spl.img flash_part.cfg 1 nor_flash.rom
```

> 注意：通常，生成rom文件的脚本会集成在SDK的makefile中，用户不需要手动在命令行输入命令来生成该文件；用户更需要关心更改配置文件来适配本地flash以及分区布局；

##  4. <a name='如何动态更新分区表数据'></a>如何动态更新分区表数据？
如果想更改设备中已经配置好的分区表，可以在设备的固件shell窗口中输入命令part：

- 创建一个新的分区：part create [name] [offset] [size]
- 更新已经存在分区：part update [name] [offset] [size]
- 删除所有分区：part del_all
- 列出所有分区：part ls
- 重置所有分区到SDK中默认的分区数据：part default

> 注意： 如果需要更改默认去，请确保各分区在内存布局上没有地址重叠；

```sh
    _    _____     ___        
   / \  |_ _\ \   / / \     
  / _ \  | | \ \ / / _ \    
 / ___ \ | |  \ V / ___ \    
/_/   \_\___|  \_/_/   \_\ 

Build:       May 11 2022 12:44:25
Version:     3.0.5
Copyright:   (c) 2020 aiva

aiva:/$ part
Usage: part [OPTIONS] [ARGS] 
   [OPTIONS]: 
      - create : create a partition,       ARGS: [name], [offset], [size] 
      - update : update a partition,       ARGS: [name], [offset], [size] 
      - del_all: delete all partitions,    ARGS: none
      - ls     : list all partions,        ARGS: none
      - default: init default part tables, ARGS: none

aiva:/$ part ls
---------- List partitions ----------
0x00080000-0x0017ffff: Name ('fw0     '), Size (1024KB)
0x00180000-0x0027ffff: Name ('fw1     '), Size (1024KB)
0x00f80000-0x00ffffff: Name ('fs      '), Size (512KB)
-------- Total 3 partitions ---------
```


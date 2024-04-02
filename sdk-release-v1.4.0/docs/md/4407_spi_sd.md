# SPI接口SD卡

## 概述

​        AI3102 SDK已经包含了基于SPI接口的SD卡驱动，且移植了FatFS，用户可直接调用FatFS提供的API来对文件进行操作，也可以调用SD卡API直接读写SD卡。

## API参考

​    对应的头文件 sdcard.h

​    为用户提供以下接口

- [sd_initialize](#sd_initialize)
- [sd_get_block_number](#sd_get_block_number)
- [sd_wait_ready](#sd_wait_ready)
- [sd_read_sector](#sd_read_sector)
- [sd_write_sector](#sd_write_sector)

### sd_initialize
<div id="sd_initialize"></div>

#### 描述

​    初始化SD卡。

#### 函数定义
```c
    int sd_initialize(void)
```
#### 参数

​    无。

#### 返回值

​    成功返回0，错误返回其他值。

### sd_get_block_number
<div id="sd_get_block_number"></div>

#### 描述

​    获取SD卡的扇区个数。

#### 函数定义
```c
    uint32_t sd_get_block_number(void)
```
#### 参数

​    无。

#### 返回值

​    SD卡的扇区个数，如果SD卡未初始化则返回0。

### sd_wait_ready
<div id="sd_wait_ready"></div>

#### 描述

​    等待SD卡就绪。

#### 函数定义
```c
    int sd_wait_ready(void)
```
#### 参数

​    无。

#### 返回值

​    就绪返回0，错误返回其他值。

### sd_read_sector
<div id="sd_read_sector"></div>

#### 描述

​    读取SD卡数据。

#### 函数定义
```c
    int sd_read_sector(uint8_t *buff, uint32_t sector, uint32_t count)
```
#### 参数

| 参数名称 | 描述                 | 输入输出 |
| -------- | -------------------- | -------- |
| buff     | 读取到的数据存放地址 | 输出     |
| sector   | 待读取数据的扇区编号 | 输入     |
| count    | 待读取数据的扇区个数 | 输入     |

#### 返回值

​    成功返回0，错误返回其他值。

### sd_write_sector
<div id="sd_write_sector"></div>

#### 描述

​    向SD卡写入数据。

#### 函数定义
```c
    int sd_write_sector(uint8_t *buff, uint32_t sector, uint32_t count)
```
#### 参数

| 参数名称 | 描述               | 输入输出 |
| -------- | ------------------ | -------- |
| buff     | 待写入数据存放地址 | 输入     |
| sector   | 待写入的扇区编号   | 输入     |
| count    | 待写入的扇区个数   | 输入     |

#### 返回值

​    成功返回0，错误返回其他值。

## 例程

### 读写SD卡

```c
uint8_t buff_wr[1024] = { 0 };
uint8_t buff_rd[1024] = { 0 };
uint32_t sect_num = 0;
uint32_t sect_count = 1024 / 512;

sd_initialize();

sd_write_sector(buff_wr, sect_num, sect_count);

sd_read_sector(buff_rd, sect_num, sect_count);
```

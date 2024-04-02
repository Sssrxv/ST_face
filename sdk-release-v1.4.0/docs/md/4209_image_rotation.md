# ROTATE接口

## 概述

    SDK中已经包含了rotate接口的驱动，对图像进行旋转操作，这里对相关接口简要说明。

## API参考

    对应的头文件 rotate.h

    为用户提供以下接口

- rotate_frame

### rotate_frame

#### 描述

    对输入的图像进行旋转

#### 函数定义
```c
    frame_buf_t *rotate_frame(frame_buf_t *buf_in, int rotate_mode, int flush_input)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame       | 输入图像 | 输入      |
| rotate_mode | 旋转模式 | 输入      |
| flush_input | 保留参数 | 输入      |

#### 返回值
    旋转结束后的图像，如果为NULL转换失败

## 数据类型

​    相关数据类型、结构体如下：

- frame_buf_t
- rotate_mode

### frame_buf_t

帧缓冲区
```c 
uint8_t *data[3];
int      stride[3];         // in bytes
int      fmt;               // frame format
int      width;             // in pixels
int      height;            // in pixels
int      channels;          // 1~3
int      used_bytes;        // currently used bytes of data buffers
int      buf_bytes;         // buffer total size in bytes;
int      is_continous;      // continous memory allocation
int      stereo_mode;       // stereo concat mode if stereo-frame
```

### rotate_mode

旋转模式
```c 
#define ROT_MOD_CLK_0               (1UL << 1)
#define ROT_MOD_CLK_90              (1UL << 2)
#define ROT_MOD_CLK_180             (1UL << 3)
#define ROT_MOD_CLK_270             (1UL << 4)
#define ROT_MOD_CLK_0_MIR           (ROT_MOD_CLK_0   | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_90_MIR          (ROT_MOD_CLK_90  | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_180_MIR         (ROT_MOD_CLK_180 | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_270_MIR         (ROT_MOD_CLK_270 | ROT_MOD_MIRROR)
```

## 例程

```c
/**
    对输入的图像frame_in旋转90°
*/
 frame_rotate_ret = rotate_frame(frame_in, ROT_MOD_CLK_90, 0);
```

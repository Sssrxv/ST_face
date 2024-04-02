# MJPEG硬件编码器

## 概述

    MJPEG编码可实现对图片的压缩以减少图片的体积。AI3102内置MJPEG硬件编码器加速MJPEG编码工作。

## API参考

    对应的头文件 mjpeg.h
    为用户提供以下接口

- mjpeg_compress


### mjpeg_compress

#### 描述

    对输入图片进行MJPEG编码。

#### 函数定义
```c
    frame_buf_t *mjpeg_compress(frame_buf_t *frame, mjpeg_comp_level_t level)
```
#### 参数

| 参数名称 | 描述                 | 输入输出 |
| :------- | -------------------- | -------- |
| frame    | 待编码的原始图片     | 输入     |
| level    | 压缩后图片的质量等级 | 输入     |

#### 返回值

    编码后的图片。

## 数据类型

    相关数据类型、结构体如下：

- mjpeg_comp_level_t：压缩后图片的质量等级
- frame_buf_t：图片类型

### mjpeg_comp_level_t

#### 描述

    经MJPEG编码压缩后图片的质量等级，质量等级越小压缩率越高，图片质量也就越低。

#### 定义

```c
typedef enum _mjpeg_comp_level_t
{
    MJPEG_COMP_LEVEL_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    MJPEG_COMP_LEVEL_HIGH          = 1,    /**< high compressing rate, %75 quality */
    MJPEG_COMP_LEVEL_LOW           = 2,    /**< low compressing rate */
    MJPEG_COMP_LEVEL_01_PERCENT    = 3,    /**< %01 quality */
    MJPEG_COMP_LEVEL_10_PERCENT    = 4,    /**< %10 quality */
    MJPEG_COMP_LEVEL_20_PERCENT    = 5,    /**< %20 quality */
    MJPEG_COMP_LEVEL_30_PERCENT    = 6,    /**< %30 quality */
    MJPEG_COMP_LEVEL_40_PERCENT    = 7,    /**< %40 quality */
    MJPEG_COMP_LEVEL_50_PERCENT    = 8,    /**< %50 quality */
    MJPEG_COMP_LEVEL_60_PERCENT    = 9,    /**< %60 quality */
    MJPEG_COMP_LEVEL_70_PERCENT    = 10,   /**< %70 quality */
    MJPEG_COMP_LEVEL_80_PERCENT    = 11,   /**< %80 quality */
    MJPEG_COMP_LEVEL_99_PERCENT    = 12,   /**< %99 quality */
    MJPEG_COMP_LEVEL_MAX                   /**< upper border (only for an internal evaluation) */
} mjpeg_comp_level_t;
```

### frame_buf_t

#### 描述

    通用图片描述类型，用来存储一幅图片。

#### 定义

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

## 例程

### 编码一幅图片

```c
frame_buf_t *frm_in = NULL;
// frm_in = ; // 使frm_in指向一幅frame_buf_t格式的图像
frame_buf_t *frm_out = NULL;
frm_out = mjpeg_compress(frm_in, MJPEG_COMP_LEVEL_60_PERCENT);
```

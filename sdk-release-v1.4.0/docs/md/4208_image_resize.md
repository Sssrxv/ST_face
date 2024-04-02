# RSZ接口

## 概述

    SDK中已经包含了resize接口的驱动，主要对图像进行格式、尺寸大小的变换以及裁剪，这里对相关接口简要说明

## API参考

    对应的头文件 rsz.h

    为用户提供以下接口

- [rsz_frame_transform](#rsz_frame_transform)
- [rsz_cvt_frame](#rsz_cvt_frame)
- [rsz_vflip_frame](#rsz_vflip_frame)
- [rsz_crop_frame](#rsz_crop_frame)
- [rsz_cvt_yuv422sp_to_rgba](#rsz_cvt_yuv422sp_to_rgba)
- [rsz_resize](#rsz_resize)
- [rsz_cvt_raw8_to_yuv422sp](#rsz_cvt_raw8_to_yuv422sp)
- [rsz_cvt_yuv_to_raw8](#rsz_cvt_yuv_to_raw8)
- [rsz_cvt_yuv422_to_yuv422sp](#rsz_cvt_yuv422_to_yuv422sp)



### rsz_frame_transform
<div id="rsz_frame_transform"></div>

#### 描述

    对输入的图像按照param的描述进行变换，包括图像尺寸、像素格式以及裁剪，将结果保存在dst输出

#### 函数定义
```c
    int rsz_frame_transform(const frame_buf_t *src, rsz_param_t *param, frame_buf_t *dst)
```
#### 参数

| 参数名称      | 描述          | 输入输出  |
| :------       | ----------    | -------- |
| src   | 输入图像           | 输入      |
| param | 输出图像的参数信息 | 输入      |
| dst   | 目标输出图像       | 输出      |

#### 返回值

    成功返回0，失败返回其他

### rsz_cvt_frame
<div id="rsz_cvt_frame"></div>

#### 描述

    按照给定的param信息，对输入图像frame进行大小、像素格式进行转换，转换结果通过函数返回值返回

#### 函数定义
```c
    frame_buf_t *rsz_cvt_frame(const frame_buf_t *frame, rsz_param_t *param)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame   | 输入图像        | 输入      |
| param   | 转换目标的描述信息 | 输入      |

#### 返回值

    转换成功后的图像，如果为NULL则转换失败

### rsz_vflip_frame
<div id="rsz_vflip_frame"></div>

#### 描述

    把输入的图像进行垂直翻转

#### 函数定义
```c
    frame_buf_t *rsz_vflip_frame(const frame_buf_t *frame)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame       | 输入图像 | 输入      |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_crop_frame
<div id="rsz_crop_frame"></div>

#### 描述

    根据坐标、尺寸剪裁输入图像的指定区域，并将剪裁得到的图像通过返回值返回

#### 函数定义
```c
    frame_buf_t *rsz_crop_frame(const frame_buf_t *frame, int cw, int ch, int x, int y)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame | 输入的图像 | 输入      |
| cw | 剪裁后输出图像的宽度 | 输入 |
| ch | 剪裁后输出图像的高度 | 输入 |
| x | 剪裁图像的起始x坐标 | 输入 |
| y | 剪裁图像的起始y坐标 | 输入 |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_cvt_yuv422sp_to_rgba
<div id="rsz_cvt_yuv422sp_to_rgba"></div>

#### 描述

    将输入的yuv422图像转换为rgba格式

#### 函数定义
```c
    frame_buf_t *rsz_cvt_yuv422sp_to_rgba(const frame_buf_t *frame)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame  | 输入的图像 | 输入 |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_resize
<div id="rsz_resize"></div>

#### 描述

    将输入的图像转换为指定的格式输出，包括图像的尺寸转换、像素格式转换

#### 函数定义
```c
    frame_buf_t *rsz_resize(const frame_buf_t *frame, int wo, int ho, rsz_cvt_fmt_t cvt)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame  | 输入的图像        | 输入      |
| wo | 输出图像的宽度 | 输入      |
| ho | 输出图像的高度 | 输入      |
| cvt | 输出图像的格式 | 输入      |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_cvt_raw8_to_yuv422sp
<div id="rsz_cvt_raw8_to_yuv422sp"></div>

#### 描述

    将输入raw8格式的图像转为yuv422输出

#### 函数定义
```c
    frame_buf_t *rsz_cvt_raw8_to_yuv422sp(const frame_buf_t *frame)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame         | 输入的图像        | 输入      |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_cvt_yuv_to_raw8
<div id="rsz_cvt_yuv_to_raw8"></div>

#### 描述

    将输入yuv格式的图像转为raw8格式输出

#### 函数定义
```c
    frame_buf_t *rsz_cvt_yuv_to_raw8(const frame_buf_t *frame)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame         | 输入的图像        | 输入 |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

### rsz_cvt_yuv422_to_yuv422sp
<div id="rsz_cvt_yuv422_to_yuv422sp"></div>

#### 描述

    将输入yuv422格式的图像转为yuv422sp格式输出

#### 函数定义
```c
    frame_buf_t *rsz_cvt_yuv422_to_yuv422sp(const frame_buf_t *frame)
```
#### 参数

| 参数名称      | 描述              | 输入输出  |
| :------------ | ----------------- | -------- |
| frame | 输入的图像 | 输入 |

#### 返回值

    转换结束后的图像，如果为NULL转换失败

## 数据类型

​    相关数据类型、结构体如下：

- [frame_buf_t](#frame_buf_t)
- [rsz_param_t](#rsz_param_t)
- [frame_fmt_t](#frame_fmt_t)
- [crop_param_t](#crop_param_t)

### frame_buf_t
<div id="frame_buf_t"></div>

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
uint32_t frame_index;
```

### rsz_param_t
<div id="rsz_param_t"></div>

rsz参数
```c 
int out_w;                  // 输出图像宽度
int out_h;                  // 输出图像宽度
frame_fmt_t out_fmt;        // 输出图像格式
int vflip;                  // 是否垂直翻转
int crop;                   // 是否crop
crop_param_t crop_param;    // crop操作参数
```

### crop_param_t
<div id="crop_param_t"></div>

图像裁剪配置
```c 
int crop_w;                 // 裁剪后图像的宽度
int crop_h;                 // 裁剪后图像的高度
int crop_xo;                // 裁剪图的起始位置x坐标
int crop_yo;                // 裁剪图的起始位置y坐标
```

### frame_fmt_t
<div id="frame_fmt_t"></div>

帧格式
```c 
FRAME_FMT_YUV422_PLANAR
FRAME_FMT_YUV422_SEMIPLANAR
FRAME_FMT_YUYV
FRAME_FMT_RGB565
FRAME_FMT_RGB666
FRAME_FMT_RGB888
FRAME_FMT_BGR888
FRAME_FMT_RGBA8888
FRAME_FMT_RAW8
FRAME_FMT_RAW10
FRAME_FMT_RAW12
FRAME_FMT_RAW16
FRAME_FMT_RAW32
FRAME_FMT_MJPEG
FRAME_FMT_I420              // YUV420 planar,       YYYY, U, V
FRAME_FMT_YV12              // YUV420 planar,       YYYY, V, U
FRAME_FMT_NV21              // YUV420 semi-planar,  YYYY, VU
FRAME_FMT_NV12              // YUV420 semi-planar,  YYYY, UV
FRAME_FMT_H264
FRAME_FMT_MAX
```

### rsz_cvt_fmt_t
<div id="rsz_cvt_fmt_t"></div>

rsz支持的转换格式
```c 
RSZ_YUVsp2YUVsp = 0,
RSZ_GRAY2GRAY   = 1,
RSZ_YUVsp2RGBA  = 2,
RSZ_YUVsp2YUYV  = 3,
RSZ_YUYV2YUVsp  = 4,
RSZ_YUYV2YUYV   = 5,
RSZ_YUYV2RGBA   = 6,
```

## 例程

```c
/**
将RGB565的640x480转换为RAW8格式、尺寸320x240输出
*/
frame_buf_t out_frame = NULL;
rsz_param_t rsz_param;
rsz_param.out_w                 = 320;
rsz_param.out_h                 = 240;
rsz_param.out_fmt               = FRAME_FMT_RAW8;
rsz_param.vflip                 = 0;
rsz_param.crop                  = 0;
out_frame_320x_240 = rsz_cvt_frame(input_frame, &rsz_param);
```

```c
/*
在输入图像的(20, 50)坐标为起始点，扣出320x240大小的图像
*/
crop_frame_ret = rsz_crop_frame(&input_frame, 320, 240, 20, 50);
```

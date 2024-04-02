# display接口

## 概述

        display接口是对硬件I8080总线的抽象，该总线由数据总线和控制总线两部分组成；该总线常用于LCD的输出，支持8/16bit的RGB输出，支持最大分辨率1920x1080；目前支持的屏幕驱动有：dvpout_ili9806、i80_alientek28、i80_jlt28010c、i80_jlt28024a、spi_lcd_ili9341、spi_lcd_ili9488、spi_lcd_st7796；
        SDK中已经包含了display接口的驱动，并且在sdk目录下example/display目录中包含有相关的使用例程，这里对相关接口简要说明。

## API参考

    对应的头文件 display.h

    为用户提供以下接口

- [display_init](#display_init)
- [display_release](#display_release)
- [display_start](#display_start)
- [display_stop](#display_stop)
- [display_send_frame](#display_send_frame)
- [display_get_frame_param](#display_get_frame_param)


### display_init
<div id="display_init"></div>

#### 描述

    根据display的接口类型初始化对应的硬件以及根据device_name查找对应的驱动，初始化设备；

#### 函数定义
```c
    display_handle_t display_init(display_interface_type_t type, const char *device_name)
```
#### 参数

| 参数名称      | 描述          | 输入输出 |
| :----------- | ----------     | -------- |
| type         | 显示接口的类型  | 输入     |
| device_name  | 设备的型号名    | 输入     |

#### 返回值
    显示设备的句柄。

### display_release
<div id="display_release"></div>

#### 描述

    释放初始化的显示设备

#### 函数定义
```c
    int display_release(display_handle_t handle)
```
#### 参数

| 参数名称    | 描述           | 输入输出 |
| :-----------| ----------    | -------- |
| handle      | 显示设备的句柄 | 输入     |

#### 返回值
    0：成功，-1：失败。

### display_start
<div id="display_start"></div>

#### 描述

    display设备开始接收数据，如果收到了数据就将一帧数据显示在LCD上；

#### 函数定义
```c
    int display_start(display_handle_t handle)
```
#### 参数

| 参数名称    | 描述           | 输入输出 |
| :-----------| ----------    | -------- |
| handle      | 显示设备的句柄 | 输入     |

#### 返回值
    0：成功，-1：失败。

### display_stop
<div id="display_stop"></div>

#### 描述

    display设备停止接收数据；

#### 函数定义
```c
    int display_stop(display_handle_t handle)
```
#### 参数

| 参数名称    | 描述           | 输入输出 |
| :-----------| ----------    | -------- |
| handle      | 显示设备的句柄 | 输入     |

#### 返回值
    0：成功，-1：失败。

### display_send_frame
<div id="display_send_frame"></div>

#### 描述

    启动接收数据后，使用该接口将数据发送到显示设备；

#### 函数定义
```c
    int display_send_frame(display_handle_t handle, frame_buf_t *frame_buf);
```
#### 参数

| 参数名称    | 描述           | 输入输出 |
| :-----------| ----------    | -------- |
| handle      | 显示设备的句柄 | 输入     |
| frame_buf   | 一帧图像数据   | 输入     |

#### 返回值
    0：成功，-1：失败。

### display_get_frame_param
<div id="display_get_frame_param"></div>

#### 描述

    获取显示设备显示一帧图像的参数；

#### 函数定义
```c
    int display_get_frame_param(display_handle_t handle, display_frame_param_t *param)
```
#### 参数

| 参数名称    | 描述           | 输入输出 |
| :-----------| ----------    | -------- |
| handle      | 显示设备的句柄 | 输入     |
| param       | LCD可显示的一帧数据格式   | 输出     |

#### 返回值
    0：成功，-1：失败。


## 数据类型

​    相关数据类型、结构体如下：

- [display_handle_t](#display_handle_t): 显示设备的句柄
- [display_interface_type_t](#display_interface_type_t): 显示接口类型
- [frame_buf_t](#frame_buf_t): 一帧图像数据
- [display_frame_param_t](#display_frame_param_t): 描述一帧数据的参数


### display_handle_t
<div id="display_handle_t"></div>

```c
    typedef void * display_handle_t;
```

### display_interface_type_t
<div id="display_interface_type_t"></div>

```c
    DISPLAY_INTERFACE_TYPE_DVPO         // DVP接口类型的显示屏
    DISPLAY_INTERFACE_TYPE_I80          // 控制总线以及数据总线类型的接口
    DISPLAY_INTERFACE_TYPE_SPI          // SPI接口类型的LCD
```
### frame_buf_t
<div id="frame_buf_t"></div>

```c
    uint8_t *data[3];       // 存储图像数据的指针
    int      stride[3];     // 一帧图像一行数据在内存中所占的字节大小
    int      fmt;           // 一帧图像的格式
    int      width;         // 一帧图像的宽度（pixels)
    int      height;        // 一帧图像的高度（pixels)
    int      channels;      // 表示图像的通道数

    int      used_bytes;    // data指针存储的字节数
    int      buf_bytes;     // data中存储的总的字节数
    int      is_continous;  // 是否连续分配内存标志
    int      stereo_mode;   // stereo concat mode if stereo-frame
    uint32_t frame_index;
```
### display_frame_param_t
<div id="display_frame_param_t"></div>

```c
    int w;          // 一帧图像的宽度
    int h;          // 一帧图像高度
    int format;     // 图像的格式
```

## 例程

```c
/**
 * 使用正点原子LCD alientek28填充一帧图像
*/
// 1、初始化I80硬件和LCD
display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_I80, "i80_alientek28");
// 2、获取LCD可支持的图像参数
display_frame_param_t param;
ret = display_get_frame_param(handle, &param);
// 3、开始接收一帧数据
ret = display_start(handle);
// 4、填充一帧合适的数据
cur0 = frame_mgr_get_one(&fparam);
// 填充cur0一帧数据
data = (uint32_t *)(cur0->data[0] + cur0->stride[0]*j);
data[i] = 0x000000FF; 
// 5、将得到的数据发送到LCD显示
display_send_frame(handle, cur0);
// 6、停止接收数据
display_stop(handle);
// 7、释放lcd以及I80驱动
display_release(handle);
```

```c
/**
 * 使用正点原子LCD alientek28显示一帧摄像头图像
*/
// 1、初始化I80硬件和LCD
display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_I80, "i80_alientek28");
// 2、获取LCD可支持的图像参数
display_frame_param_t param;
ret = display_get_frame_param(handle, &param);
// 3、开始接收一帧数据
ret = display_start(handle);
// 4、从摄像头拿一帧数据
ret = videoin_get_frame(context, &videoin_frame, VIDEOIN_WAIT_FOREVER);
// 5、调整适合显示的格式
frame_buf_t * frame_send = rsz_cvt_yuv422sp_to_rgba(frame_yuv422sp);
// 6、将得到的数据发送到LCD显示
display_send_frame(handle, frame_send);
// 7、停止接收数据
display_stop(handle);
// 8、释放lcd以及I80驱动
display_release(handle);
```

详细参考example/display目录下的相关文件。

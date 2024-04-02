#ifndef _INDEV_FT6336U_H_
#define _INDEV_FT6336U_H_

#if USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#define TouchWidth              319
#define TouchHeight             479

#define FT6336U_I2C_ADDR        0x38

// Touch Parameter
#define FT6336U_PRES_DOWN       0x2
#define FT6336U_COORD_UD        0x1

/**
 * Write 0x40 into this reg, switch to factory mode, and read value 0x00.
*/
#define FT6336U_ADDR_DEVICE_MODE            0x00        // mode switch. 
typedef enum {
    working_mode = 0b000, 
    factory_mode = 0b100, 
} DEVICE_MODE_Enum;

#define FT6336U_ADDR_TD_STATUS              0x02        // report touched point count. max is 2.
#define FT6336_ADDR_TOUCH1_XH               0x03        // [7,6]: 1th point touched flag. [3~0]:1th point x-axis high 4bits.
#define FT6336_ADDR_TOUCH1_XL               0x04        // 1th point X-axis low byte.
#define FT6336_ADDR_TOUCH1_YH               0x05        // [7~4]:1th point ID, [3~0]:1th point Y-Axis high 4bits.
#define FT6336_ADDR_TOUCH1_YL               0x06        // 1th point Y-axis low byte.

#define FT6336U_ADDR_TOUCH1_WEIGHT          0x07
#define FT6336U_ADDR_TOUCH1_MISC            0x08
#define FT6336U_ADDR_TOUCH2_XH              0x09        // [7,6]: 2th point touched flag. [3~0]:1th point x-axis high 4bits.
#define FT6336U_ADDR_TOUCH2_XL              0x0A        // 2th point X-axis low byte.
#define FT6336U_ADDR_TOUCH2_YH              0x0B        // [7~4]:2th point ID, [3~0]:1th point Y-Axis high 4bits.
#define FT6336U_ADDR_TOUCH2_YL              0x0C        // 2th point Y-axis low byte.
#define FT6336U_ADDR_TOUCH2_WEIGHT          0x0D
#define FT6336U_ADDR_TOUCH2_MISC            0x0E

#define FT6336U_ADDR_THRESHOLD              0x80        // touch threshold.
#define FT6336U_ADDR_FILTER_COE             0x85
#define FT6336U_ADDR_CTRL                   0x86
typedef enum {
    keep_active_mode = 0, 
    switch_to_monitor_mode = 1, 
} CTRL_MODE_Enum; 

#define FT6336U_ADDR_TIME_ENTER_MONITOR     0x87
#define FT6336U_ADDR_ACTIVE_MODE_RATE       0x88
#define FT6336U_ADDR_MONITOR_MODE_RATE      0x89
#define FT6336U_ADDR_FREQ_HOPPING_EN        0x8B

#define FT6336U_ADDR_RADIAN_VALUE           0x91
#define FT6336U_ADDR_OFFSET_LEFT_RIGHT      0x92
#define FT6336U_ADDR_OFFSET_UP_DOWN         0x93
#define FT6336U_ADDR_DISTANCE_LEFT_RIGHT    0x94
#define FT6336U_ADDR_DISTANCE_UP_DOWN       0x95
#define FT6336U_ADDR_DISTANCE_ZOOM          0x96      // ID_G_TEST_MODE_FILTER
#define FT6336U_ADDR_DISTANCE_ZOOM          0x9F      // chiper middle byte. default 0x26.

#define FT6336U_ADDR_CHIP_ID_L              0xA0      // chiper low btye. 0:FT6236G, 1:FT6336G, 2:FT6336U, 3:FT6426.
#define FT6336U_ADDR_LIBRARY_VERSION_H      0xA1      // APP lib version hight byte.
#define FT6336U_ADDR_LIBRARY_VERSION_L      0xA2      // APP lib version low byte.
#define FT6336U_ADDR_CHIP_ID_H              0xA3      // chiper hight btye. default 0x64
#define FT6336U_ADDR_G_MODE                 0xA4      // if delye INT low level time.
typedef enum {
    pollingMode = 0, 
    triggerMode = 1, 
} G_MODE_Enum; 

#define FT6336U_ADDR_POWER_MODE             0xA5    // power comsume mode. 0:P_ACTIVE, 1:P_MONITOR, 2:P_STANDBY,3:P_HIBERNATE
#define FT6336U_ADDR_FIRMARE_ID             0xA6    // fw version. default 0x00.
#define FT6336U_ADDR_FOCALTECH_ID           0xA8    // vendor id, default 0x11.
#define FT6336U_ADDR_IS_CALLING             0xAD    // calling flag.
#define FT6336U_ADDR_FACTORY_MODE           0xAE    // factory mode type. 0:F_NORMAL, 1:F_TESTMODE_1, 2:F_TESTMODE_2
#define FT6336U_ADDR_RELEASE_CODE_ID        0xAF    // public version release id. default 0x11.
#define FT6336U_FACE_DEC_MODE               0xB0    // near detect enable. default 0x00.

/**
 * write 0xAA, 0x55 into this reg,goto update flow.
 * 0x00:InFo mode.
 * 0x01:normal work mode.
 * 0x02:factory mode.
 * 0x03:auto calibration mode.
*/
#define FT6336U_ADDR_STATE                  0xBC    // work mode.

#endif

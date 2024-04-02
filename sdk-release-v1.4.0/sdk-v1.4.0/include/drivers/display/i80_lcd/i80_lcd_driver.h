#ifndef _I80_LCD_DRIVER_H_
#define _I80_LCD_DRIVER_H_

#include <stdint.h>
#include "spi.h"
#include "sysctl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _i80_out_fmt_t {
    I80_OUT_FMT_INVALID         = -1,
    I80_OUT_FMT_8BIT_RGB444     = 0x0,
    I80_OUT_FMT_8BIT_RGB565     = 0x1,
    I80_OUT_FMT_8BIT_RGB666     = 0x2,
    I80_OUT_FMT_8BIT_RGB888     = 0x3,
    I80_OUT_FMT_8BIT_RGBA888    = 0x4,
    I80_OUT_FMT_16BIT_RGB444    = 0x5,
    I80_OUT_FMT_16BIT_RGB565    = 0x6,
    I80_OUT_FMT_16BIT_RGB666_1  = 0x7,
    I80_OUT_FMT_16BIT_RGB666_2  = 0x8,
    I80_OUT_FMT_16BIT_RGB666_3  = 0x9,
    I80_OUT_FMT_16BIT_RGB666_4  = 0xa,
    I80_OUT_FMT_16BIT_RGB888_1  = 0xb,
    I80_OUT_FMT_16BIT_RGB888_2  = 0xc,
    I80_OUT_FMT_16BIT_RGBA8888  = 0xd,
    I80_OUT_FMT_MAX
} i80_out_fmt_t;

/**
 * @brief       I80_LCD configuration param
 */
typedef struct display_i80_params {
    int             width;            /* image width    */
    int             height;           /* image height   */
    i80_out_fmt_t   out_fmt;          /* data format  */
} __attribute__ ((packed, aligned(4))) display_i80_params_t;

/** display_dev_driver_t */
typedef struct i80_lcd_driver {
    const char              *name;          /** dirver name */
    void                    *context;       /** pointer to  driver private context */
    int                     backlight_pin;
    int                     reset_pin;

    int  (*init)(struct i80_lcd_driver *dev_driver);
    int  (*suspend)(struct i80_lcd_driver *dev_driver);
    int  (*resume)(struct i80_lcd_driver *dev_driver);
    void (*set_backlight)(struct i80_lcd_driver *dev_driver,uint32_t value);
    void (*set_reset_pin)(struct i80_lcd_driver *dev_driver, uint32_t value);
    int  (*get_interface_param)(struct i80_lcd_driver *dev_driver, display_i80_params_t *param);
} i80_lcd_driver_t;

#ifdef __cplusplus
}
#endif


#endif

#ifndef _DVPOUT_LCD_DRIVER_H_
#define _DVPOUT_LCD_DRIVER_H_

#include <stdint.h>
#include "spi.h"
#include "sysctl.h"

#ifdef __cplusplus
extern "C" {
#endif


/* c_dvpo_pol */
#define DVPO_VS_POL_LEVEL                                   (0UL)
#define DVPO_VS_POL_PULSE                                   (1UL)
#define DVPO_HS_POL_LEVEL                                   (0UL << 4)
#define DVPO_HS_POL_PULSE                                   (1UL << 4)
#define DVPO_DE_POL_LEVEL                                   (0UL << 8)
#define DVPO_DE_POL_PULSE                                   (1UL << 8)
#define DVPO_CLK_POS_EDGE                                   (0UL << 12)
#define DVPO_CLK_NEG_EDGE                                   (1UL << 12)

/* c_dvpo_dis */
#define DVPO_VSYNC_DIS_EN                                   (1UL)       // Vsync disable: true
#define DVPO_VSYNC_DIS_DIS                                  (0UL)       // Vsync disable: false
#define DVPO_HSYNC_DIS_EN                                   (1UL << 4)  // Hsync disable: true
#define DVPO_HSYNC_DIS_DIS                                  (0UL << 4)  // Hsync disable: false
#define DVPO_DE_DIS_EN                                      (1UL << 8)  // DE disable: true
#define DVPO_DE_DIS_DIS                                     (0UL << 8)  // DE disable: false

/* c_dvpo_vsb */
#define DVPO_HS_VSB_EN                                      (1UL << 4)
#define DVPO_HS_VSB_DIS                                     (0UL << 4)
#define DVPO_DE_VSB_EN                                      (1UL << 8)
#define DVPO_DE_VSB_DIS                                     (0UL << 8)

/**
 * @brief       DVPOUT_LCD configuration param
 */
typedef struct display_dvpout_params {
    uint16_t  width;            /* image width    */
    uint16_t  height;           /* image height   */
    uint16_t  hlw;              /* h-blank width  */
    uint16_t  vlw;              /* v-blank width  */
    uint16_t  hbp;              /* h-back porch   */
    uint16_t  vbp;              /* v-back porch   */
    uint16_t  hfp;              /* h-front porch  */
    uint16_t  vfp;              /* v-front porch  */
    uint32_t  vs_pol;           /* vsync polarity */
    uint32_t  hs_pol;           /* hsync polarity */
    uint32_t  de_pol;           /* DE polarity    */
    uint32_t  vsync_dis;        /* vsync disable  */
    uint32_t  hsync_dis;        /* hsync disable  */
    uint32_t  de_dis;           /* DE disable     */
    uint32_t  dvpo_fmt;         /* data format    */
    uint32_t  hsync_vsb;        /* hsync-keep during vsync blank */
    uint32_t  de_vsb;           /* DE-keep during vsync blank */
} __attribute__ ((packed, aligned(4))) display_dvpout_params_t;

typedef enum _dvpo_fmt_t {
    DVPO_FMT_RAW8 = 0,
    DVPO_FMT_RAW16_YUYV = 1,
    DVPO_FMT_RAW16_RGB565 = 2,
    DVPO_FMT_RGBA8888 = 3,
    DVPO_FMT_MAX,
} dvpo_fmt_t;


typedef struct dvpout_lcd_driver {
    const char              *name;          /** dirver name */
    void                    *context;       /** pointer to  driver private context */
    int                     backlight_pin;
    int                     reset_pin;

    spi_device_num_t        spi_dev;
    spi_chip_select_t       spi_cs;
    sysctl_clock_t          spi_clk;

    int  (*init)(struct dvpout_lcd_driver *dev_driver);
    int  (*suspend)(struct dvpout_lcd_driver *dev_driver);
    int  (*resume)(struct dvpout_lcd_driver *dev_driver);
    void  (*set_backlight)(struct dvpout_lcd_driver *dev_driver,uint32_t value);
    void (*set_reset_pin)(struct dvpout_lcd_driver *dev_driver, uint32_t value);
    int  (*get_interface_param)(struct dvpout_lcd_driver *dev_driver, display_dvpout_params_t *param);
} dvpout_lcd_driver_t;


#ifdef __cplusplus
}
#endif


#endif

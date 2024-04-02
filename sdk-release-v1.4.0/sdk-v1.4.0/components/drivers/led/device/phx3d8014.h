#ifndef __PHX3D8014_H__
#define __PHX3D8014_H__

#include <stdint.h>
// #include "dvpin.h"
#include "i2c.h"

#if USE_RTOS
#include "FreeRTOS.h"
#include "timers.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif


/* =================================================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/* =================================================================================== */
#define REG_UNION_DEFINE(x) \
        typedef union\
        {\
            uint8_t bytes;\
            CfgRegAddr##x data;\
        }Typ_CfgRegAddr##x;

#define UNION_REG(x) Typ_CfgRegAddr##x
/* =================================================================================== */

/**
 * @brief private data for 36xx led device
 * @note see led_driver.h
 */
typedef struct {
    i2c_device_number_t i2c_num;          /** i2c number */
    uint8_t i2c_addr;                     /** i2c address */
    uint8_t i2c_addr_width;               /** i2c address width */
    uint32_t i2c_clk;                     /** i2c clk */
    uint8_t power_pin;                    /** power pin number */
    uint8_t led_index;                    /** which led is bind to this 36xx device */
    int use_strobe;                       /** use sensor strobe or gpio to trigger 36xx */
#if USE_RTOS
    TimerHandle_t timer;                  /** software timer which will be used to handle led timeout */
    pthread_mutex_t mutex;                /** internal mutex */
#endif
} phx3d8014_led_t;


typedef struct 
{
    uint8_t GRST        :1;
    uint8_t SPI_MODE    :1;
    uint8_t SEL_IREF    :1;
    uint8_t APC_EN      :1;
    uint8_t IB_SEL      :1;
    uint8_t TAPC        :2;
    uint8_t SLEEP_MODE  :1;
} __attribute__ ((packed)) CfgRegAddr00;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED        :6;
    uint8_t EN_ITO          :1;
    uint8_t LD_SEL          :1;
} __attribute__ ((packed)) CfgRegAddr01;
/* =================================================================================== */
typedef struct 
{
    uint8_t ISW;
} __attribute__ ((packed)) CfgRegAddr02;
/* =================================================================================== */
typedef struct 
{
    uint8_t IB_FIX;
} __attribute__ ((packed)) CfgRegAddr03;
/* =================================================================================== */
typedef struct 
{
    uint8_t I_LD2;
} __attribute__ ((packed)) CfgRegAddr04;
/* =================================================================================== */
typedef struct 
{
    uint8_t IB_ERR;
} __attribute__ ((packed)) CfgRegAddr05;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED        :3;
    uint8_t Vth_PD_ERR_H    :5;
} __attribute__ ((packed)) CfgRegAddr06;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED        :3;
    uint8_t Vth_PD_ERR_L    :5;
} __attribute__ ((packed)) CfgRegAddr07;
/* =================================================================================== */
typedef struct 
{
    uint8_t Vth_ITO_ERR_H   :4;
    uint8_t Vth_ITO_ERR_L   :4;
} __attribute__ ((packed)) CfgRegAddr08;
/* =================================================================================== */
typedef struct 
{
    uint8_t I_ITO   :4;
    uint8_t I_AUX   :4;
} __attribute__ ((packed)) CfgRegAddr09;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED   :3;
    uint8_t VCALB_PD   :5;
} __attribute__ ((packed)) CfgRegAddr0A;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED1   :2;
    uint8_t RESERVED2   :1;
    uint8_t OC          :3;
    uint8_t UV          :2;
} __attribute__ ((packed)) CfgRegAddr0B;
/* =================================================================================== */
typedef struct 
{
    uint8_t IB_CALB;
} __attribute__ ((packed)) CfgRegAddr0C;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED1   :2;
    uint8_t TEMP        :2;
    uint8_t PD          :2;
    uint8_t AUX         :2;
} __attribute__ ((packed)) CfgRegAddr0D;
/* =================================================================================== */
typedef struct 
{
    uint8_t TEMP;
} __attribute__ ((packed)) CfgRegAddr0E;
/* =================================================================================== */
typedef struct 
{
    uint8_t PD;
} __attribute__ ((packed)) CfgRegAddr0F;
/* =================================================================================== */
typedef struct 
{
    uint8_t AUX;
} __attribute__ ((packed)) CfgRegAddr10;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED        :2;
    uint8_t ITO_ERR_FLAG    :1;
    uint8_t ITH_ERR_FLAG    :1;
    uint8_t PD_FLAG         :1;
    uint8_t OVER_TEMP_FLAG  :1;
    uint8_t OC_FLAG         :1;
    uint8_t UV_FLAG         :1;
} __attribute__ ((packed)) CfgRegAddr11;
/* =================================================================================== */
typedef struct 
{
    uint8_t PDER_LT     :1;
    uint8_t OT_LT       :1;
    uint8_t OC_LT       :1;
    uint8_t UV_LT       :1;
    uint8_t PD_SD_EN    :1;
    uint8_t OT_SD_EN    :1;
    uint8_t OC_SD_EN    :1;
    uint8_t UV_SD_EN    :1;
} __attribute__ ((packed)) CfgRegAddr12;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED    :5;
    uint8_t ITO_SD_EN   :1;
    uint8_t ITO_ERR_LT  :1;
    uint8_t ICLAMP      :1;
} __attribute__ ((packed)) CfgRegAddr13;
/* =================================================================================== */
typedef struct 
{
    uint8_t Device_ID;
} __attribute__ ((packed)) CfgRegAddr14;
/* =================================================================================== */
typedef struct 
{
    uint8_t TR_ASSIST_LD2   :4;
    uint8_t TR_ASIST        :4;
} __attribute__ ((packed)) CfgRegAddr15;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED    :6;
    uint8_t SD_SPI      :1;
    uint8_t SD_IIC      :1;
} __attribute__ ((packed)) CfgRegAddr17;
/* =================================================================================== */
typedef struct 
{
    uint8_t RESERVED1   :2;
    uint8_t ITO_ERR_H_SD:1;
    uint8_t PD_ERR_H_SD :1;
    uint8_t RESERVED2   :2;
    uint8_t ITO_ERR_L_SD:1;
    uint8_t PD_ERR_L_SD :1;
} __attribute__ ((packed)) CfgRegAddr18;
/* =================================================================================== */
REG_UNION_DEFINE(00);
REG_UNION_DEFINE(01);
REG_UNION_DEFINE(02);
REG_UNION_DEFINE(03);
REG_UNION_DEFINE(04);
REG_UNION_DEFINE(05);
REG_UNION_DEFINE(06);
REG_UNION_DEFINE(07);
REG_UNION_DEFINE(08);
REG_UNION_DEFINE(09);
REG_UNION_DEFINE(0A);
REG_UNION_DEFINE(0B);
REG_UNION_DEFINE(0C);
REG_UNION_DEFINE(0D);
REG_UNION_DEFINE(0E);
REG_UNION_DEFINE(0F);
REG_UNION_DEFINE(10);
REG_UNION_DEFINE(11);
REG_UNION_DEFINE(12);
REG_UNION_DEFINE(13);
REG_UNION_DEFINE(14);
REG_UNION_DEFINE(15);
REG_UNION_DEFINE(17);
REG_UNION_DEFINE(18);
/* =================================================================================== */
// int     ocp8681_init(i2c_device_number_t i2c_num, uint8_t pin, bool gpio_control);
// void    ocp8681_power_on(void);
// void    ocp8681_power_off(void);
// void    ocp8681_suspend(void);
// void    ocp8681_resume(void);
// int     ocp8681_flash_bright(void *param);
// int     ocp8681_set_flash_timeout_ms(int timeout_ms);
// void    ocp8681_get_current_range_wrapper(float *min, float *max, float *step);
// int     ocp8681_set_flash_timeout_ms(int timeout_ms);
// int     ocp8681_test(i2c_device_number_t i2c_num, uint8_t pin, bool gpio_control);
// void    ocp8681_old_test();
/* =================================================================================== */
#ifdef __cplusplus
}
#endif
#endif

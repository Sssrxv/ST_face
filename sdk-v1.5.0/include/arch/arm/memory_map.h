#ifndef _MEMORY_MAP_H
#define _MEMORY_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
/* DMA */
#define DMA_BASE_ADDR            (0x20000000U)

/* Resize */
#define RSZ_BASE_ADDR            (0x20001000U)

/* MJPEG */
#define MJPEG_BASE_ADDR          (0x20002000U)

/* HDR */
#define HDR_BASE_ADDR            (0x20003000U)

/* ROT */
#define ROT_BASE_ADDR            (0x20004000U)

/* VDE */
#define VDE_BASE_ADDR            (0x20008000U)

/* VTE */
#define VTE_BASE_ADDR            (0x20009000U)

/* XNN */
#define XNN_BASE_ADDR            (0x2000F000U)

/* ISP_CORE */
#define ISP_CORE_ADDR            (0x20010000U)

/* DDRC */
#define DDRC_BASE_ADDR           (0x20020000U)

/* DVPIN0~2 */
#define DVPRX0_BASE_ADDR         (0x20030000U)
#define DVPRX1_BASE_ADDR         (0x20031000U)
#define DVPRX2_BASE_ADDR         (0x20032000U)

/* ISP_MIPI0~2 */
#define ISP_MIPI0_ADDR           (0x20034000U)
#define ISP_MIPI1_ADDR           (0x20035000U)
#define ISP_MIPI2_ADDR           (0x20036000U)

/* DVPOUT */
#define DVPTX_BASE_ADDR          (0x2003F000U)

/* PWM0~7 */
#define PWM0_BASE_ADDR           (0x20040000U)
#define PWM1_BASE_ADDR           (0x20041000U)
#define PWM2_BASE_ADDR           (0x20042000U)
#define PWM3_BASE_ADDR           (0x20043000U)
#define PWM4_BASE_ADDR           (0x20044000U)
#define PWM5_BASE_ADDR           (0x20045000U)
#define PWM6_BASE_ADDR           (0x20046000U)
#define PWM7_BASE_ADDR           (0x20047000U)

/* PWM8~11*/
#define PWM8_BASE_ADDR           (0x20048000U)
#define PWM9_BASE_ADDR           (0x20049000U)
#define PWM10_BASE_ADDR          (0x2004a000U)
#define PWM11_BASE_ADDR          (0x2004b000U)

/* PWC0~3 */
#define PWC0_BASE_ADDR           (0x20050000U)
#define PWC1_BASE_ADDR           (0x20051000U)
#define PWC2_BASE_ADDR           (0x20052000U)
#define PWC3_BASE_ADDR           (0x20053000U)
/* PWC4~5*/
#define PWC4_BASE_ADDR           (0x20054000U)
#define PWC5_BASE_ADDR           (0x20055000U)

/* UART0~4 */
#define UART0_BASE_ADDR          (0x20058000U)
#define UART1_BASE_ADDR          (0x20059000U)
#define UART2_BASE_ADDR          (0x2005A000U)
#define UART3_BASE_ADDR          (0x2005B000U)
#define UART4_BASE_ADDR          (0x2005C000U)

/* I2C0~4 */
#define I2CM0_BASE_ADDR          (0x20060000U)
#define I2CM1_BASE_ADDR          (0x20061000U)
#define I2CM2_BASE_ADDR          (0x20062000U)
#define I2CM3_BASE_ADDR          (0x20063000U)
#define I2CM4_BASE_ADDR          (0x20064000U)

/* I2S */
#define I2S_BASE_ADDR            (0x20070000U)

/* GPIO */
#define GPIO_BASE_ADDR           (0x20074000U)

/* SPI0~1, QSPIS */
#define QSPIM0_BASE_ADDR         (0x20078000U)
#define SPIM1_BASE_ADDR          (0x20079000U)
#define QSPIS_BASE_ADDR          (0x2007B000U)


/* SMU (System management Unit) */
#define SMU_BASE_ADDR            (0x200E0000U)

/* RTC */
#define RTC_BASE_ADDR            (0x200E1000U)

/* ETH */
#define ETH_BASE_ADDR            (0x200F0000U)

/* USB */
#define USB_BASE_ADDR            (0x20100000U)

/* SPACC */
#define SPACC_BASE_ADDR          (0x20300000U)

/* PKA */
#define PKA_BASE_ADDR            (0x20380000U)

/* TRNG */
#define TRNG_BASE_ADDR           (0x20390000U)

/* CODEC */
#define CODEC_BASE_ADDR           (0x2000C000)

//#define DDRPHY_BASE_ADDR         (0x20031000U)

#define DDR_BASE_ADDR            (0x80000000U)

#define LCDRGB_BASE_ADDR         DVPTX_BASE_ADDR
#define LCDMCU_BASE_ADDR         DVPTX_BASE_ADDR


#ifdef __cplusplus
}
#endif

#endif /* _MEMORY_MAP_H */


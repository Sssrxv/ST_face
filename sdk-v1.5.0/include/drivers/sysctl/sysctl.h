#ifndef _DRIVER_SYSCTL_H
#define _DRIVER_SYSCTL_H

#include <stdint.h>
#include "memory_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _sysctl_pll_t
{
    SYSCTL_PLL0 = 0,
    SYSCTL_PLL1,
    SYSCTL_PLL_MAX
} sysctl_pll_t;

typedef enum _sysctl_clock_source_t
{
    SYSCTL_SOURCE_IN0,
    SYSCTL_SOURCE_PLL0,
    SYSCTL_SOURCE_PLL1,
    SYSCTL_SOURCE_ACLK,
    SYSCTL_SOURCE_MAX
} sysctl_clock_source_t;

typedef enum _sysctl_dma_channel_t
{
    SYSCTL_DMA_CHANNEL_0,
    SYSCTL_DMA_CHANNEL_1,
    SYSCTL_DMA_CHANNEL_2,
    SYSCTL_DMA_CHANNEL_3,
    SYSCTL_DMA_CHANNEL_4,
    SYSCTL_DMA_CHANNEL_5,
    SYSCTL_DMA_CHANNEL_MAX
} sysctl_dma_channel_t;

typedef enum _sysctl_dma_select_t
{
    SYSCTL_DMA_SELECT_SSI0_RX_REQ,
    SYSCTL_DMA_SELECT_SSI0_TX_REQ,
    SYSCTL_DMA_SELECT_SSI1_RX_REQ,
    SYSCTL_DMA_SELECT_SSI1_TX_REQ,
    SYSCTL_DMA_SELECT_SSI2_RX_REQ,
    SYSCTL_DMA_SELECT_SSI2_TX_REQ,
    SYSCTL_DMA_SELECT_SSI3_RX_REQ,
    SYSCTL_DMA_SELECT_SSI3_TX_REQ,
    SYSCTL_DMA_SELECT_I2C0_RX_REQ,
    SYSCTL_DMA_SELECT_I2C0_TX_REQ,
    SYSCTL_DMA_SELECT_I2C1_RX_REQ,
    SYSCTL_DMA_SELECT_I2C1_TX_REQ,
    SYSCTL_DMA_SELECT_I2C2_RX_REQ,
    SYSCTL_DMA_SELECT_I2C2_TX_REQ,
    SYSCTL_DMA_SELECT_I2C3_RX_REQ,
    SYSCTL_DMA_SELECT_I2C3_TX_REQ,
    SYSCTL_DMA_SELECT_I2C4_RX_REQ,
    SYSCTL_DMA_SELECT_I2C4_TX_REQ,
    SYSCTL_DMA_SELECT_I2C5_RX_REQ,
    SYSCTL_DMA_SELECT_I2C5_TX_REQ,
    SYSCTL_DMA_SELECT_UART0_RX_REQ,
    SYSCTL_DMA_SELECT_UART0_TX_REQ,
    SYSCTL_DMA_SELECT_UART1_RX_REQ,
    SYSCTL_DMA_SELECT_UART1_TX_REQ,
    SYSCTL_DMA_SELECT_UART2_RX_REQ,
    SYSCTL_DMA_SELECT_UART2_TX_REQ,
    SYSCTL_DMA_SELECT_UART3_RX_REQ,
    SYSCTL_DMA_SELECT_UART3_TX_REQ,
    SYSCTL_DMA_SELECT_I2S0_TX_REQ,
    SYSCTL_DMA_SELECT_I2S0_RX_REQ,
    SYSCTL_DMA_SELECT_I2S1_TX_REQ,
    SYSCTL_DMA_SELECT_I2S1_RX_REQ,
    SYSCTL_DMA_SELECT_MAX
} sysctl_dma_select_t;

/**
 * @brief      System controller clock id
 */
typedef enum _sysctl_clock_t
{
    SYSCTL_CLOCK_VPE_T,
    SYSCTL_CLOCK_XNN_T,
    SYSCTL_CLOCK_XNN,
    SYSCTL_CLOCK_UMC_T,
    SYSCTL_CLOCK_DMA,
    SYSCTL_CLOCK_RSZ,
    SYSCTL_CLOCK_JPEG,
    SYSCTL_CLOCK_SRAM1,
    SYSCTL_CLOCK_SRAM2,
    SYSCTL_CLOCK_VDE,
    SYSCTL_CLOCK_VTE,
    SYSCTL_CLOCK_DVPI0,
    SYSCTL_CLOCK_DVPO,
    SYSCTL_CLOCK_DDR,
    SYSCTL_CLOCK_PWM0,
    SYSCTL_CLOCK_PWM1,
    SYSCTL_CLOCK_PWM2,
    SYSCTL_CLOCK_PWM3,
    SYSCTL_CLOCK_PWM4,
    SYSCTL_CLOCK_PWM5,
    SYSCTL_CLOCK_PWM6,
    SYSCTL_CLOCK_PWM7,
    SYSCTL_CLOCK_PWM8,
    SYSCTL_CLOCK_PWM9,
    SYSCTL_CLOCK_PWM10,
    SYSCTL_CLOCK_PWM11,
    SYSCTL_CLOCK_PWC0,
    SYSCTL_CLOCK_PWC1,
    SYSCTL_CLOCK_PWC2,
    SYSCTL_CLOCK_PWC3,
    SYSCTL_CLOCK_PWC4,
    SYSCTL_CLOCK_PWC5,
    SYSCTL_CLOCK_UART0,
    SYSCTL_CLOCK_UART1,
    SYSCTL_CLOCK_UART2,
    SYSCTL_CLOCK_UART3,
    SYSCTL_CLOCK_UART4,
    SYSCTL_CLOCK_I2C0,
    SYSCTL_CLOCK_I2C1,
    SYSCTL_CLOCK_I2C2,
    SYSCTL_CLOCK_I2C3,
    SYSCTL_CLOCK_I2C4,
    SYSCTL_CLOCK_I2S0,
    SYSCTL_CLOCK_GPIO,
    SYSCTL_CLOCK_SPI0,
    SYSCTL_CLOCK_SPI1,
    SYSCTL_CLOCK_SPIS,
    SYSCTL_CLOCK_RTC,
    SYSCTL_CLOCK_ETH,
    SYSCTL_CLOCK_USB,
    SYSCTL_CLOCK_GLO_TIMER, /* SCU global timer */
    SYSCTL_CLOCK_LOC_TIMER, /* SCU local timer  */
    SYSCTL_CLOCK_WDT,
    SYSCTL_CLOCK_CPU,
    SYSCTL_CLOCK_ISP_T,
    SYSCTL_CLOCK_ISP,
    SYSCTL_CLOCK_ROT,
    SYSCTL_CLOCK_SPACC,
    SYSCTL_CLOCK_PKA,
    SYSCTL_CLOCK_TRNG,
    SYSCTL_CLOCK_MIPI0,
    SYSCTL_CLOCK_MIPI1,
    SYSCTL_CLOCK_MIPI2,
    SYSCTL_CLOCK_HDR,
    SYSCTL_CLOCK_VCE_T,
    SYSCTL_CLOCK_VCE,
    SYSCTL_CLOCK_MAX,
} sysctl_clock_t;

/**
 * @brief      System controller clock select id
 */
typedef enum _sysctl_clock_select_t
{
    SYSCTL_CLOCK_SELECT_HCLK,
    SYSCTL_CLOCK_SELECT_LCLK,
    SYSCTL_CLOCK_SELECT_PCLK,
    SYSCTL_CLOCK_SELECT_CCLK,
    SYSCTL_CLOCK_SELECT_SCLK,
    SYSCTL_CLOCK_SELECT_XCLK,
    SYSCTL_CLOCK_SELECT_VCLK,
    SYSCTL_CLOCK_SELECT_MCLK,
    SYSCTL_CLOCK_SELECT_ICLK,
    SYSCTL_CLOCK_SELECT_USB_RCLK,
    SYSCTL_CLOCK_SELECT_USB_TCLK,
    SYSCTL_CLOCK_SELECT_ETH_RCLK,   // Reference clk
    SYSCTL_CLOCK_SELECT_ETH_MCLK,   // MII clk
    SYSCTL_CLOCK_SELECT_DVPO_PCLK,
    SYSCTL_CLOCK_SELECT_MIPI_RCLK,
    SYSCTL_CLOCK_SELECT_MIPI_BCLK,
    SYSCTL_CLOCK_SELECT_MIPI_TCLK,
    //SYSCTL_CLOCK_SELECT_RTC_CLK,
    SYSCTL_CLOCK_SELECT_SENS0_RCLK,
    SYSCTL_CLOCK_SELECT_SENS1_RCLK,
    SYSCTL_CLOCK_SELECT_I2S_MCLK,
    SYSCTL_CLOCK_SELECT_I2S_BCLK,
    SYSCTL_CLOCK_SELECT_ECLK,
    SYSCTL_CLOCK_SELECT_ALL,
    SYSCTL_CLOCK_SELECT_MAX
} sysctl_clock_select_t;

typedef enum _sysctl_clk_div_t {
    CDIV_HCLK,
    CDIV_LCLK, 
    CDIV_PCLK, 
    CDIV_CCLK, 
    CDIV_SCLK,
    CDIV_XCLK,
    CDIV_VCLK,
    CDIV_MCLK,
    CDIV_ICLK,
    CDIV_USB_RCLK, 
    CDIV_ETH_RCLK,
    CDIV_ETH_MCLK,
    CDIV_DVPO_PCLK,
    CDIV_MIPI_RCLK,
    CDIV_MIPI_BCLK,
    CDIV_MIPI_TCLK,
    CDIV_I2S_MCLK,
    CDIV_I2S_BCLK,
    CDIV_SENS0_RCLK,
    CDIV_SENS1_RCLK,
    CDIV_ECLK,
    CDIV_MAX,
} sysctl_clk_div_t;

/**
 * @brief      System controller clock threshold id
 */
typedef enum _sysctl_threshold_t
{
    SYSCTL_THRESHOLD_ACLK,
    SYSCTL_THRESHOLD_APB0,
    SYSCTL_THRESHOLD_APB1,
    SYSCTL_THRESHOLD_APB2,
    SYSCTL_THRESHOLD_SRAM0,
    SYSCTL_THRESHOLD_SRAM1,
    SYSCTL_THRESHOLD_SPI0,
    SYSCTL_THRESHOLD_SPI1,
    SYSCTL_THRESHOLD_SPI2,
    SYSCTL_THRESHOLD_TIMER0,
    SYSCTL_THRESHOLD_TIMER1,
    SYSCTL_THRESHOLD_TIMER2,
    SYSCTL_THRESHOLD_I2S0,
    SYSCTL_THRESHOLD_I2S1,
    SYSCTL_THRESHOLD_I2C0,
    SYSCTL_THRESHOLD_I2C1,
    SYSCTL_THRESHOLD_I2C2,
    SYSCTL_THRESHOLD_WDT,
    SYSCTL_THRESHOLD_MAX
} sysctl_threshold_t;

/**
 * @brief      System controller reset control id
 */
typedef enum _sysctl_reset_t
{
    SYSCTL_RESET_VPE_T = 0,
    SYSCTL_RESET_XNN_T,
    SYSCTL_RESET_UMC_T,
    SYSCTL_RESET_DMA,
    SYSCTL_RESET_RSZ,
    SYSCTL_RESET_JPEG,
    SYSCTL_RESET_SRAM1,
    SYSCTL_RESET_SRAM2,
    SYSCTL_RESET_XNN,
    SYSCTL_RESET_VDE,
    SYSCTL_RESET_VTE,
    SYSCTL_RESET_DVPI0,
    SYSCTL_RESET_MIPI0,
    SYSCTL_RESET_MIPI1,
    SYSCTL_RESET_MIPI2,
    SYSCTL_RESET_DVPO,
    SYSCTL_RESET_DDR,
    SYSCTL_RESET_PWM0,
    SYSCTL_RESET_PWM1,
    SYSCTL_RESET_PWM2,
    SYSCTL_RESET_PWM3,
    SYSCTL_RESET_PWM4,
    SYSCTL_RESET_PWM5,
    SYSCTL_RESET_PWM6,
    SYSCTL_RESET_PWM7,
    SYSCTL_RESET_PWM8,
    SYSCTL_RESET_PWM9,
    SYSCTL_RESET_PWM10,
    SYSCTL_RESET_PWM11,
    SYSCTL_RESET_PWC0,
    SYSCTL_RESET_PWC1,
    SYSCTL_RESET_PWC2,
    SYSCTL_RESET_PWC3,
    SYSCTL_RESET_PWC4,
    SYSCTL_RESET_PWC5,
    SYSCTL_RESET_UART0,
    SYSCTL_RESET_UART1,
    SYSCTL_RESET_UART2,
    SYSCTL_RESET_UART3,
    SYSCTL_RESET_UART4,
    SYSCTL_RESET_I2C0,
    SYSCTL_RESET_I2C1,
    SYSCTL_RESET_I2C2,
    SYSCTL_RESET_I2C3,
    SYSCTL_RESET_I2C4,
    SYSCTL_RESET_I2S0,
    SYSCTL_RESET_GPIO,
    SYSCTL_RESET_SPI0,
    SYSCTL_RESET_SPI1,
    SYSCTL_RESET_SPIS,
    SYSCTL_RESET_RTC,
    SYSCTL_RESET_ETH,
    SYSCTL_RESET_USB,
    SYSCTL_RESET_ISP_T,
    SYSCTL_RESET_ISP,
    SYSCTL_RESET_HDR,
    SYSCTL_RESET_ROT,
    SYSCTL_RESET_SPACC,
    SYSCTL_RESET_PKA,
    SYSCTL_RESET_TRNG,
    SYSCTL_RESET_VCE_T,
    SYSCTL_RESET_VCE,
    SYSCTL_RESET_MAX
} sysctl_reset_t;

/**
 * @brief      System reset status
 */
typedef enum _sysctl_reset_enum_status
{
    SYSCTL_RESET_STATUS_HARD,
    SYSCTL_RESET_STATUS_SOFT,
    SYSCTL_RESET_STATUS_WDT,
    SYSCTL_RESET_STATUS_MAX,
} sysctl_reset_enum_status_t;

/**
 * @brief 　TODO
 * 
 * @return int >0:is 3101, <=0: 3102
 */
int is_chip_3101(void);

/**
 * @brief       Enable clock for peripheral
 *
 * @param[in]   clock       The clock to be enable
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int sysctl_clock_enable(sysctl_clock_t clock);

/**
 * @brief       Enable clock for peripheral
 *
 * @param[in]   clock       The clock to be disable
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int sysctl_clock_disable(sysctl_clock_t clock);


/**
 * @brief       Sysctl clock set clock select
 *
 * @param[in]   which       Which clock select to set
 * @param[in]   select      The clock select value
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int sysctl_clock_set_clock_select(sysctl_clock_select_t which, int select);

/**
 * @brief       Sysctl clock get clock select
 *
 * @param[in]   which  Which clock select to get
 *
 * @return      The clock select value
 *     - Other  Value of clock select
 *     - -1     Fail
 */
int sysctl_clock_get_clock_select(sysctl_clock_select_t which);


/**
 * @brief       Get base clock frequency by clock id
 *
 * @param[in]   clock       The clock id
 *
 * @return      The clock frequency
 */
uint32_t sysctl_clock_get_freq(sysctl_clock_t clock);

/**
 * @brief       Reset device by reset controller
 *
 * @param[in]   reset       The reset signal
 */
void sysctl_reset(sysctl_reset_t reset);




/**
 * @brief       Get the time start up to now
 *
 * @return      The time of microsecond
 */
uint64_t sysctl_get_time_us(void);

/**
 * @brief       Get the time start up to now
 *
 * @return      The time of millisecond
 */
uint64_t sysctl_get_time_ms(void);


typedef enum {
    IO_SWITCH_I2CM0 = 0,
    IO_SWITCH_I2CM1,
    IO_SWITCH_I2CM2,
    IO_SWITCH_I2CM3,
    IO_SWITCH_I2CM4,
    IO_SWITCH_UART0,
    IO_SWITCH_UART1,
    IO_SWITCH_UART2,
    IO_SWITCH_UART3,
    IO_SWITCH_UART4,
    IO_SWITCH_GPIO0,
    IO_SWITCH_GPIO1,
    IO_SWITCH_GPIO2,
    IO_SWITCH_GPIO3,
    IO_SWITCH_GPIO4,
    IO_SWITCH_GPIO5,
    IO_SWITCH_GPIO6,
    IO_SWITCH_GPIO7,
    IO_SWITCH_GPIO8,
    IO_SWITCH_GPIO9,
    IO_SWITCH_GPIO10,
    IO_SWITCH_GPIO11,
    IO_SWITCH_GPIO12,
    IO_SWITCH_GPIO13,
    IO_SWITCH_GPIO14,
    IO_SWITCH_GPIO15,
    IO_SWITCH_GPIO16,
    IO_SWITCH_GPIO17,
    IO_SWITCH_GPIO18,
    IO_SWITCH_GPIO19,
    IO_SWITCH_GPIO20,
    IO_SWITCH_GPIO21,
    IO_SWITCH_GPIO22,
    IO_SWITCH_GPIO23,
    IO_SWITCH_GPIO24,
    IO_SWITCH_GPIO25,
    IO_SWITCH_GPIO26,
    IO_SWITCH_GPIO27,
    IO_SWITCH_GPIO28,
    IO_SWITCH_GPIO29,
    IO_SWITCH_GPIO30,
    IO_SWITCH_GPIO31,
    IO_SWITCH_PWM0  ,
    IO_SWITCH_PWM1  ,
    IO_SWITCH_PWM2  ,
    IO_SWITCH_PWM3  ,
    IO_SWITCH_PWM4  ,
    IO_SWITCH_PWM5  ,
    IO_SWITCH_PWM6  ,
    IO_SWITCH_PWM7  ,
    IO_SWITCH_PWM8  ,
    IO_SWITCH_PWM9  ,
    IO_SWITCH_PWM10 ,
    IO_SWITCH_PWM11 ,
    IO_SWITCH_PWC0  ,
    IO_SWITCH_PWC1  ,
    IO_SWITCH_PWC2  ,
    IO_SWITCH_PWC3  ,
    IO_SWITCH_PWC4  ,
    IO_SWITCH_PWC5  ,
    IO_SWITCH_DVPO_MCU,
    IO_SWITCH_JTAG,
    IO_SWITCH_DVPI,
    IO_SWITCH_DVPO_D,
    IO_SWITCH_ETH,
    IO_SWITCH_I2S,
    IO_SWITCH_MCLK0,
    IO_SWITCH_MCLK1,
    IO_SWITCH_SPIM,
    IO_SWITCH_QSPIS,
    IO_SWITCH_SPIM_CS0,
    IO_SWITCH_SPIM_CS1,
    IO_SWITCH_SPIM_CS2,
    IO_SWITCH_MAX,
} sysctl_io_switch_t;

#define CPU_IDLE()										        \
    __asm volatile ( "wfi" );                                   \
    __asm volatile ( "DSB" );                                   \
    __asm volatile ( "ISB" );

#define CPU_IRQ_DISABLE()										\
	__asm volatile ( "CPSID i" );							    \
	__asm volatile ( "DSB" );								    \
	__asm volatile ( "ISB" );

#define CPU_IRQ_ENABLE()										\
	__asm volatile ( "CPSIE i" );							    \
	__asm volatile ( "DSB" );								    \
	__asm volatile ( "ISB" );

typedef enum _mclk_id_t {
    MCLK_ID0 = 0,
    MCLK_ID1,
} mclk_id_t;

typedef enum _power_gtn_id_t{
    POWER_GTN_XNN = 0,
    POWER_GTN_VPE = 1,
    POWER_GTN_ISP = 2,
    POWER_GTN_MAX,
} power_gtn_id_t;

typedef enum _power_gtn_op_t {
    POWER_GTN_OP_OFF = 0,
    POWER_GTN_OP_ON,
} power_gtn_op_t;

void sysctl_set_io_switch(sysctl_io_switch_t swi, uint8_t val);
void sysctl_set_clk_div(sysctl_clk_div_t clk_div, uint8_t div);
uint8_t sysctl_get_clk_div(sysctl_clk_div_t clk_div);
uint32_t sysctl_get_sens_mclk(mclk_id_t id);
uint32_t sysctl_set_sens_mclk(mclk_id_t id, uint32_t freq);
int sysctl_set_power_gating(power_gtn_id_t id, power_gtn_op_t op);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_SYSCTL_H */

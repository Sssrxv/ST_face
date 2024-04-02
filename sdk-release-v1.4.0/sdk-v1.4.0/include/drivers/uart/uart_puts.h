#ifndef _UART_PUTS_H_
#define _UART_PUTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "uart/uart.h"
#include "SEGGER_RTT.h"

typedef struct _uart_puts_config {
    int dev;                        /* UART device number */
    uint32_t baudr;                 /* UART baud rate */
    int enable_rtt;                 /* Whether to enable RTT, 1 - Enable, 0 - Disable */
    int rtt_mode;                   /* SEGGER_RTT_MODE_NO_BLOCK_SKIP, 
                                     * SEGGER_RTT_MODE_NO_BLOCK_TRIM, 
                                     * SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL 
                                     */
} uart_puts_config_t;

#define UART_PUTS_CFG_INITIALIZER           \
    ( ( (uart_puts_config_t)                \
    {                                       \
        .dev        = 0,                    \
        .baudr      = 460800,               \
        .enable_rtt = 0,                    \
        .rtt_mode   = 1,                    \
    }                                       \
    )                                       \
    )

#ifdef CONFIG_LOG_ENABLE
/** 
 * @brief  uart_puts_init, initialize uart puts with default configuration
 *         UART0 - 460800 - 8/1/None
 */
void uart_puts_init(void);

/** 
 * @brief  uart_puts_set_config, config the default uart device number and baudrate
 * @param[in] cfg   cfg pointer
 * 
 */
int uart_puts_set_config(uart_puts_config_t *cfg);

/** 
 * @brief  uart_puts_get_config 
 * @param[out] cfg   cfg pointer
 *
 */
int uart_puts_get_config(uart_puts_config_t *cfg);

/** 
 * @brief  uart_puts, print chars with uart
 * 
 * @param[in] buf   buffer pointer of chars
 * @param[in] len   buffer length in bytes
 * 
 * @return sent bytes count
 */
int uart_puts(const char* buf, int len);

/** 
 * @brief  uart_tstc, test whether there is available data on configured uart port
 * 
 * @return 0 - no data, 1 - data available  
 */
int uart_tstc(void);

/** 
 * @brief  uart_getc , get one char from uart port
 * 
 * @return the current char on uart port 
 */
int uart_getc(void);

/** 
 * @brief  uart_putc, print one char on uart port
 * 
 * @param[in] c     char to print
 * 
 * @return  sent bytes count 
 */
int uart_putc(int c);

#else
#define uart_puts_init
#define uart_puts(a, b)
#define uart_tstc()
#define uart_getc()
#define uart_putc(c)
#define uart_puts_config(a, b)
#endif // CONFIG_LOG_ENABLE


#ifdef __cplusplus
}
#endif

#endif

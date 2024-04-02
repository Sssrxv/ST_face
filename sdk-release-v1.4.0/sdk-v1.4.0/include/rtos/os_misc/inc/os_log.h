#ifndef __OS_LOG_H__
#define __OS_LOG_H__

#include <stdint.h>
#include <stdbool.h>
#include "syslog.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: comments below macro to disable async log */
//#define OS_ASYNC_LOG

typedef enum _os_log_mode_t {
    OS_LOG_UART_MODE = 0,
    OS_LOG_USB_MODE  = 1,
    OS_LOG_RTT_MODE  = 2,
} os_log_mode_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief  get the curruent os log mode
 * 
 * @return os_log_mode_t  
 */
/* --------------------------------------------------------------------------*/
os_log_mode_t os_log_get_mode(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  set os log mode
 * 
 * @param mode  log mode
 * 
 * @return  0: OK, -1: Fail 
 */
/* --------------------------------------------------------------------------*/
int  os_log_set_mode(os_log_mode_t mode);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  printf function with RTOS lock
 * 
 * @param fmt
 * @param ...
 * 
 * @return  0: OK, -1: Fail 
 */
/* --------------------------------------------------------------------------*/
int  os_log_printf(const char *fmt, ...);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  enable log printf
 */
/* --------------------------------------------------------------------------*/
void os_log_enable(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  disable log printf
 */
/* --------------------------------------------------------------------------*/
void os_log_disable(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  whether log printf is enabled
 * 
 * @return true/false  
 */
/* --------------------------------------------------------------------------*/
bool os_log_is_enabled(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  set uart parameters of log printf
 * 
 * @param uart_dev  uart device number
 * @param baudr     baud rate
 * 
 * @return  0: OK, -1: Fail 
 */
/* --------------------------------------------------------------------------*/
int os_log_set_uart_dev_baudr(int uart_dev, int baudr);
/* --------------------------------------------------------------------------*/
/** 
 * @brief  set RTT mode param of log printf
 * 
 * @param rtt_mode
 * 
 * @return  0: OK, -1: Fail 
 */
/* --------------------------------------------------------------------------*/
int os_log_set_rtt_mode(int rtt_mode);

#ifdef __cplusplus
}
#endif 

#endif // __OS_LOG_H__

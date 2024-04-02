#ifndef XNN_LOG_H_
#define XNN_LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include "platform.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _xnn_log_level
{
    XNN_LOG_NONE    = 0, /*!< No log output */
    XNN_LOG_ERROR   = 1, /*!< Critical errors, software module can not recover on its own */
    XNN_LOG_WARN    = 2, /*!< Error conditions from which recovery measures have been taken */
    XNN_LOG_INFO    = 3, /*!< Information messages which describe normal flow of events */
    XNN_LOG_DEBUG   = 4, /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    XNN_LOG_VERBOSE = 5, /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
    XNN_LOG_MAX
} xnn_log_level_t ;

int xnn_log_init();
int xnn_log_release();
int xnn_log_printf(xnn_log_level_t level, const char* file, 
        unsigned line, const char* func, const char* format, ...);

int xnn_log_set_level(xnn_log_level_t level);

xnn_log_level_t xnn_log_get_level();

#if XNN_LOG
#define XNN_LOGE(expr ...)  xnn_log_printf(XNN_LOG_ERROR,   __FILE__, __LINE__, __func__, expr)
#define XNN_LOGW(expr ...)  xnn_log_printf(XNN_LOG_WARN,    __FILE__, __LINE__, __func__, expr)
#define XNN_LOGI(expr ...)  xnn_log_printf(XNN_LOG_INFO,    __FILE__, __LINE__, __func__, expr)
#define XNN_LOGD(expr ...)  xnn_log_printf(XNN_LOG_DEBUG,   __FILE__, __LINE__, __func__, expr)
#define XNN_LOGV(expr ...)  xnn_log_printf(XNN_LOG_VERBOSE, __FILE__, __LINE__, __func__, expr)
#else
#define XNN_LOGE(expr ...)
#define XNN_LOGW(expr ...)
#define XNN_LOGI(expr ...)
#define XNN_LOGD(expr ...)
#define XNN_LOGV(expr ...)
#endif


#ifdef __cplusplus
}
#endif

#endif

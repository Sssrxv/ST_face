#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum log_level
{
    LOG_NONE    = 0, /*!< No log output */
    LOG_ERROR   = 1, /*!< Critical errors, software module can not recover on its own */
    LOG_WARN    = 2, /*!< Error conditions from which recovery measures have been taken */
    LOG_INFO    = 3, /*!< Information messages which describe normal flow of events */
    LOG_DEBUG   = 4, /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    LOG_VERBOSE = 5, /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} log_level_t;

typedef void (*log_callback_t)(const char* buffer, size_t buffer_size);

void syslog_set_log_level(log_level_t new_log_level);

log_level_t syslog_get_log_level();

void syslog_register_callback(log_callback_t cb);

void syslog_unregister_callback();

int syslog(log_level_t level, const char* tag, const char* format, ...);

int syslog_enable_sleep(void);

int syslog_disable_sleep(void);

#ifndef __FILENAME__
// #define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILENAME__ __FILE__
#endif

#define LOGE(tag, format, ...)  do {syslog(LOG_ERROR, tag, "%s:%d " format, __FILENAME__, __LINE__, ##__VA_ARGS__);} while (0)
#define LOGW(tag, format, ...)  do {syslog(LOG_WARN, tag, format, ##__VA_ARGS__);} while (0)
#define LOGI(tag, format, ...)  do {syslog(LOG_INFO, tag, format, ##__VA_ARGS__);} while (0)
#define LOGD(tag, format, ...)  do {syslog(LOG_DEBUG, tag, format, ##__VA_ARGS__);} while (0)
#define LOGV(tag, format, ...)  do {syslog(LOG_VERBOSE, tag, format, ##__VA_ARGS__);} while (0)

/* check return value */
#define CHECK_RET(r)                                                   \
    do {                                                               \
        if ((r) < 0) {                                                 \
            LOGE(__func__, "(%d) return error code %d.",__LINE__, r);  \
            return r;                                                  \
        }                                                              \
    } while (0)

#define configASSERT(x)                                                 \
    if ((x) == 0)                                                       \
    {                                                                   \
        LOGE("configASSERT: ", "(%s:%d)", __FILENAME__, __LINE__);      \
        for (;;)                                                        \
            ;                                                           \
    }

#define AIVA_CHECK(TAG, cond, action, msg) if (!(cond)) {                       \
        LOGE(TAG,"%s:%d (%s): %s", __FILENAME__, __LINE__, __FUNCTION__, msg);  \
        action;                                                                 \
        }

#define AIVA_MEM_CHECK(TAG,  cond, action)  AIVA_CHECK(TAG, cond, action, "Memory exhausted")

#define AIVA_NULL_CHECK(TAG, cond, action)  AIVA_CHECK(TAG, cond, action, "Got NULL Pointer")


#ifdef __cplusplus
}
#endif


#endif /* _SYSLOG_H */


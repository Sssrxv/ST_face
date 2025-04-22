#ifndef _ISP_LOG_H_
#define _ISP_LOG_H_
#include "syslog.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _isp_log_level
{
    ISP_LOG_NONE    = 0, /*!< No log output */
    ISP_LOG_ERROR   = 1, /*!< Critical errors, software module can not recover on its own */
    ISP_LOG_WARN    = 2, /*!< Error conditions from which recovery measures have been taken */
    ISP_LOG_INFO    = 3, /*!< Information messages which describe normal flow of events */
    ISP_LOG_DEBUG   = 4, /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ISP_LOG_VERBOSE = 5, /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} isp_log_level;

//#define CONFIG_ISP_LOG_LEVEL (ISP_LOG_VERBOSE)
#ifdef USE_FACELOCK_V2  // joey: temp disable isp log for facelock_v2
#define CONFIG_ISP_LOG_LEVEL (ISP_LOG_NONE)
#else
#define CONFIG_ISP_LOG_LEVEL (ISP_LOG_WARN)
#endif

#ifdef CONFIG_LOG_ENABLE
// #define ISP_LOG_ERROR(format, ...)      do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_ERROR)      LOG_PRINTF(LOG_FORMAT(E, format), "isp", ##__VA_ARGS__); } while(0)
// #define ISP_LOG_WARN(format, ...)       do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_WARN)       LOG_PRINTF(LOG_FORMAT(W, format), "isp", ##__VA_ARGS__); } while(0)
// #define ISP_LOG_INFO(format, ...)       do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_INFO)       LOG_PRINTF(LOG_FORMAT(I, format), "isp", ##__VA_ARGS__); } while(0)
// #define ISP_LOG_DEBUG(format, ...)      do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_DEBUG)      LOG_PRINTF(LOG_FORMAT(D, format), "isp", ##__VA_ARGS__); } while(0)
// #define ISP_LOG_VERBOSE(format, ...)    do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_VERBOSE)    LOG_PRINTF(LOG_FORMAT(V, format), "isp", ##__VA_ARGS__); } while(0)
#define ISP_LOG_ERROR(format, ...)      do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_ERROR)      LOGE("isp", format, ##__VA_ARGS__); } while(0)
#define ISP_LOG_WARN(format, ...)       do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_WARN)       LOGW("isp", format, ##__VA_ARGS__); } while(0)
#define ISP_LOG_INFO(format, ...)       do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_INFO)       LOGI("isp", format, ##__VA_ARGS__); } while(0)
#define ISP_LOG_DEBUG(format, ...)      do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_DEBUG)      LOGD("isp", format, ##__VA_ARGS__); } while(0)
#define ISP_LOG_VERBOSE(format, ...)    do {if (CONFIG_ISP_LOG_LEVEL >= ISP_LOG_VERBOSE)    LOGV("isp", format, ##__VA_ARGS__); } while(0)
#else
#define ISP_LOG_ERROR(format, ...)      
#define ISP_LOG_WARN(format, ...)       
#define ISP_LOG_INFO(format, ...)       
#define ISP_LOG_DEBUG(format, ...)      
#define ISP_LOG_VERBOSE(format, ...)    
#endif



#ifdef __cplusplus
}
#endif


#endif

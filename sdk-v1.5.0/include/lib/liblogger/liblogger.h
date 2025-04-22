/**
 * @file
 * @brief  simple cpp logger
 * @note why design this logger: we wanto dynamic set log level for our app for easy debug;
 *       also we wan this logger support control range, so different app module can has their
 *       own log level
 * @note why not use LOG* in syslog.h? LOG* can not support dynamic config at runtime
 * @note why not use XNN_LOG* which already support dynamic config?
 *       (1) XNN_LOG* is currently belong to xnn runtime which may be seperate out from fw some day
 *       (2) XNN_LOG* not support multi instance so we can not set different log level for different module
 * @note logger support callback, you can redirect log to file by specify a callback
 * @note this logger is not interly support print __FILE__ __LINE__ as LOG* or XNN_LOG* since we
 *       can not easy to get compile time info at run time, so user should send __FILE__ __LINE__ as
 *       params when they wanto print these info
 */
#ifndef __LIB_LOGGER_H__
#define __LIB_LOGGER_H__

#ifdef USE_RTOS
#include "liblock_guard.h"
#endif
#include <stddef.h> // for size_t
#include <cstdarg> // for va_list

namespace fw_lib {

typedef enum
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,
} log_level_t;

typedef void (*logger_callback_t)(const char* buffer, size_t buffer_size);

class Logger
{
public:
    Logger(log_level_t log_level);
    Logger();
    void set_log_level(log_level_t new_log_level);
    void set_callback(logger_callback_t cb);
public:
    int error(const char* tag, const char* format, ...);
    int warning(const char* tag, const char* format, ...);
    int info(const char* tag, const char* format, ...);
    int debug(const char* tag, const char* format, ...);
    int verbose(const char* tag, const char* format, ...);

private:
    int log(const char* tag, const char* color, const char* format, va_list args);

private:
    // FIXME: fw not correctly implement vsnprintf, so there is
    // chance to make memory access out of range, here we simply use
    // large buffer size to avoid this case
    char buffer[1024];
#ifdef USE_RTOS
    pthread_mutex_t buffer_mutex;
#endif
    //
    log_level_t log_level;
    logger_callback_t logger_callback;
};

}

#endif

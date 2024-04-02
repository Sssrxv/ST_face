/**
 * @file
 * @brief  lock guard work with pthread_mutex_t to simplify usage of lock
 */
#ifndef _LIB_LOCK_GUARD_H_
#define _LIB_LOCK_GUARD_H_

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX_portable.h"

#include <stdbool.h>

namespace fw_lib {

class LockGuard
{
public:
    explicit LockGuard(pthread_mutex_t *mutex);
    ~LockGuard();
private:
    pthread_mutex_t *mutex;
};

class TimedLockGuard
{
public:
    explicit TimedLockGuard(pthread_mutex_t *mutex);
    ~TimedLockGuard();

    bool try_lock_for(unsigned int timeout_ms);

private:
    pthread_mutex_t *mutex;
    bool lock_success;
};

}

#endif

#ifndef _XNN_LOCKER_H_
#define _XNN_LOCKER_H_

#include <time.h>
#include <map>
#include <string.h>
#include <stdint.h>
#if WITH_PTHREAD
#if USE_RTOS
extern "C"
{
    // for pthread_mutex_t
    #include "FreeRTOS_POSIX/pthread.h"
    #include "atomic.h"
    #include "FreeRTOS_POSIX/unistd.h"
}
#else
#include <pthread.h>
#include <unistd.h>
#endif
#endif

#include "xnn_log.h"

namespace xnn
{

class XnnLocker
{
public:
    static XnnLocker& get_instance()
    {
        static XnnLocker instance;

        return instance;
    }

    // timeout_ms: 0  for wait forever
    //             >0 for wait for the timeout_ms microseconds
    int lock(unsigned int timeout_ms = 0)
    {
        int ret = 0;
#if WITH_PTHREAD
        if (timeout_ms > 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000 * 1000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_mutex_timedlock(&m_mutex, &ts);
            if (ret != 0) {
                XNN_LOGE("xnn locker timedlock fail\n");
            }
        } else {
            ret = pthread_mutex_lock(&m_mutex);
            if (ret != 0) {
                XNN_LOGE("xnn locker fail\n");
            }
        }
#endif
        return ret;
    }

    int unlock()
    {
        int ret = 0;
#if WITH_PTHREAD
        ret = pthread_mutex_unlock(&m_mutex);
        if (ret != 0) {
            XNN_LOGE("xnn unlock fail\n");
        }
#endif
        return ret;
    }

public:
    XnnLocker(XnnLocker const&)    = delete;
    void operator=(XnnLocker const&)  = delete;

private:
    XnnLocker()
    {
#if WITH_PTHREAD
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        int ret = pthread_mutex_init(&m_mutex, &attr);
        if (ret != 0) {
            XNN_LOGE("pthread mutex init xnn mutex fail\n");
        }
#endif
    }


    ~XnnLocker()
    {
#if WITH_PTHREAD
        pthread_mutex_destroy(&m_mutex);
#endif
    }

private:
#if WITH_PTHREAD
    pthread_mutex_t m_mutex;
#endif
};



} // namespace xnn




#endif

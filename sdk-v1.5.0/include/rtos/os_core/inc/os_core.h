#ifndef __OS_TASK_H__
#define __OS_TASK_H__

#include <stdint.h>
#include "frame_mgr.h"

#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX_portable.h"
#include "FreeRTOS_POSIX/utils.h"

typedef enum {
    OS_WAIT_FOREVER = -1,
    OS_WAIT_NONE    =  0,
} os_wait_option_t;

#define __MAKE_FUNC(x)            void *work_thread_entry_ ## x (void *param)
#define THREAD_ENTRY(x)         __MAKE_FUNC(x)
#define __THREAD_ENTRY_NAME(x)    work_thread_entry_ ## x
#define THREAD_ENTRY_NAME(x)    __THREAD_ENTRY_NAME(x)

// macro to remove unused param warning
#define PARAM_UNUSED(x)        (void)x

#define OS_MSG_MAGIC           (0x20200607)

#endif // __OS_TASK_H__

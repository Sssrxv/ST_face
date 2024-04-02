#ifndef __OS_ASYNC_CB_TASK_H__
#define __OS_ASYNC_CB_TASK_H__

#include <stdbool.h>
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef int (*async_cb_func_t)(void *param);

int os_async_get_curr_handlers(void);

int os_async_get_max_handlers(void);

bool os_async_is_busy(void);

int os_async_commit_handler(async_cb_func_t callback, void *param);

int os_async_task_entry(void);

int os_async_task_clear_all_handlers();


#ifdef __cplusplus
}
#endif 

#endif

#include "shell.h"
#include "aiva_malloc.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"

void mem_usage(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    Shell *shell = shellGetCurrent();
    char *pcWriteBuffer = pvPortMalloc(2048);
    int usgae = aiva_get_heap_usage();
    uint32_t idle_usgae = 0;

    vTaskGetRunTimeStats2(pcWriteBuffer, &idle_usgae);

    shellPrint(shell, "heap usage:%d%%\n", usgae);
    shellPrint(shell, "cpu usage:%d%%\n", (100 - idle_usgae));
    vPortFree(pcWriteBuffer);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
mem_usage, mem_usage, print memory usage);

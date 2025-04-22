#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"

void ls_task(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    Shell *shell = shellGetCurrent();
    char *pcWriteBuffer = pvPortMalloc(2048);
#if (defined configUSE_TRACE_FACILITY  && configUSE_TRACE_FACILITY == 1)
    shellPrint(shell, "*******************************************\n");
    shellPrint(shell, "Name          State    Pri Stack-Remain ID\n");
    shellPrint(shell, "*******************************************\n");
    vTaskList(pcWriteBuffer);
    shellWriteString(shell, pcWriteBuffer);
    shellPrint(shell, "\n");
#else
    shellPrint(shell, "configUSE_TRACE_FACILITY is not enabled!\n");
#endif

#if (defined configGENERATE_RUN_TIME_STATS && configGENERATE_RUN_TIME_STATS == 1)
    vTaskGetRunTimeStats(pcWriteBuffer);
    shellPrint(shell, "*******************************************\n");
    shellPrint(shell, "TaskName       ScheduleCount   Usage\n");
    shellPrint(shell, "*******************************************\n");
    shellWriteString(shell, pcWriteBuffer);
    shellPrint(shell, "\n");
    shellPrint(shell, "*******************************************\n");
#else
    shellPrint(shell, "configGENERATE_RUN_TIME_STATS is not enabled!\n");
#endif
    vPortFree(pcWriteBuffer);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
ls_task, ls_task, list tasks);

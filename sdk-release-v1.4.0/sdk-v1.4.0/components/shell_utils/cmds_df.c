#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"
#include "fs.h"
#include <string.h>
#include <stdio.h>

void cmds_df(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    int size;
    int capacity;
	int percentage;

    size = fs_size();
    capacity = fs_capacity();
    Shell *shell = shellGetCurrent();
    char *pcWriteBuffer = pvPortMalloc(256);
    if (pcWriteBuffer == NULL)
    {
        shellPrint(shell, "No enough memory\n");
    }
    memset(pcWriteBuffer, 0, 256);

    percentage = ((float)size / (float) capacity) * 100;

    shellPrint(shell, "Filesystem      Used            Available       Usage\n");

    if (percentage > 0)
    {
        sprintf(pcWriteBuffer, "%s\t\t%d\t\t%d\t%d%%\r\n", "/", size, capacity - size, percentage);
    }
    else
    {
        sprintf(pcWriteBuffer, "%s\t\t%d\t\t%d\t<1%%\r\n", "/", size, capacity - size);
    }

    shellWriteString(shell, pcWriteBuffer);
    shellPrint(shell, "\n");

    vPortFree(pcWriteBuffer);

}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
df, cmds_df, df);

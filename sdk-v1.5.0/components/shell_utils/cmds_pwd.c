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
#include "fs_file.h"
#include <string.h>
#include <stdio.h>

void pwd_dir(char * pcWriteBuffer)
{
    char * g_path = fs_get_curr_gpath();
    
    sprintf(pcWriteBuffer, "%s\r\n", g_path);

    return ;
}

void cmds_pwd(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    Shell *shell = shellGetCurrent();
    char *pcWriteBuffer = pvPortMalloc(256);
    if (pcWriteBuffer == NULL)
    {
        shellPrint(shell, "No enough memory\n");
    }
    memset(pcWriteBuffer, 0, 256);

    pwd_dir(pcWriteBuffer);
    shellWriteString(shell, pcWriteBuffer);
    shellPrint(shell, "\n");

    vPortFree(pcWriteBuffer);

}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
pwd, cmds_pwd, pwd);

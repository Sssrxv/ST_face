#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"
#include "lfs.h"
#include "fs.h"
#include "fs_file.h"
#include <string.h>
#include <stdio.h>

static void cmds_remove_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: rm [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - file or dir name: string\r\n"
            );
}

void cmds_rm(int argc, char *argv[])
{
    int ret;

    if (argc != 2) {
        cmds_remove_help();
        return ;
    }

    Shell *shell = shellGetCurrent();
    
    if (strncmp(argv[1], ".", 1) == 0 && strlen(argv[1]) == 1)
    {
        shellPrint(shell, "rm: refusing to remove '.' or '..' directory: skipping '%s'\n", argv[1]);
        return ;
    } 
    else if (strncmp(argv[1], "..", 2) == 0 && strlen(argv[1]) == 2)
    {
        shellPrint(shell, "rm: refusing to remove '.' or '..' directory: skipping '%s'\n", argv[1]);
        return ;
    }
    else if (strncmp(argv[1], "/", 1) == 0 && strlen(argv[1]) == 1)
    {
        shellPrint(shell, "rm: it is dangerous to operate recursively on '/'\n");
        return ;
    }

    ret = fs_remove_file(argv[1], strlen(argv[1]) + 1);
    if (ret < 0)
    {
        if (ret == LFS_ERR_NOTEMPTY) {
            shellPrint(shell, "rm: %s: Dir is not empty:%d\n", argv[1],ret);
        }
        else {
            shellPrint(shell, "rm: %s: No such file or directory:%d\n", argv[1],ret);
        }
    }
    
    shellPrint(shell, "\n");
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
rm, cmds_rm, rm);

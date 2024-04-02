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

static void cmds_mkdir_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: mkdir [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - directory name: string\r\n"
            );
}

void cmds_mkdir(int argc, char *argv[])
{
    int ret;

    if (argc != 2) {
        cmds_mkdir_help();
        return ;
    }

    ret = fs_mkdir(argv[1]);
    if (ret < 0) {
        Shell *shell = shellGetCurrent();
        if (ret == LFS_ERR_EXIST) {
            shellPrint(shell, "mkdir error: can't create directory '%s', file exists",
                    argv[1]);
        }
        else {
            shellPrint(shell, "mkdir error: %d", ret);
        }
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
mkdir, cmds_mkdir, mkdir);

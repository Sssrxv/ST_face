#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"
#include "fs_file.h"
#include <string.h>
#include <stdio.h>

#define PRINT_BUFFER    2048

static void cmds_ls_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: ls [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - directory name:  | string\r\n"
            );
}

static int ls_dir(char *pcWriteBuffer, const char* path, int len, int is_show_all)
{
    fs_dir_t * fs_dir = NULL;
    struct lfs_info info;
    char fullpath[PATH_LEN] = {0};
    int err;
    int ret = 0;
    int offset = 0;
    char * g_path = fs_get_curr_gpath();

    /* absolute path */
    if (strncmp(path, "/", 1) == 0) {
        if(len > PATH_LEN) {
            ret = -2;
            return ret;
        }
        memcpy(fullpath, path, len);
    }
    /* relative path */
    else {
        /** to simplify the process, if . and .. sysbols are 
         *  included in the path, we don`t count real path length.
         */
        offset = strlen(g_path);
        if(offset + len > PATH_LEN) {
            ret = -2;
            return ret;
        }
        if(strlen(g_path) == 1) {
            snprintf(fullpath, PATH_LEN, "%s%s", g_path, path);
        }
        else {
            snprintf(fullpath, PATH_LEN, "%s%s%s", g_path, "/", path);
        }
    }

    fs_dir = fs_dir_open(fullpath, &err);
    if (err == LFS_ERR_OK) {
        int readret;
        do {
            readret = fs_dir_read(fs_dir, &info);
            if (readret == 1) {
                /* if filename or dir name begin with '.', it will be invisible. */
                if(strncmp(info.name, ".", 1) != 0 
                    || (strncmp(info.name, ".", 1) == 0 && strlen(info.name) == 1)
                    || (strncmp(info.name, "..", 2) == 0 && strlen(info.name) == 2)
                    || (strncmp(info.name, ".", 1) == 0 && is_show_all == true)) {
                    /* type 1:file, type 2: dir */
                    if (info.type == 2) {
                        snprintf(pcWriteBuffer, PRINT_BUFFER, "%d\t%s%s\r\n",(int)info.size, info.name, "/");
                        pcWriteBuffer += strlen( pcWriteBuffer ); 
                    }
                    else {
                        snprintf(pcWriteBuffer, PRINT_BUFFER, "%d\t%s\r\n",(int)info.size, info.name);
                        pcWriteBuffer += strlen( pcWriteBuffer );
                    }
                }
            }
            else if(readret < 0) {
                ret = -3;
                break;
            }
        } while (readret != 0);
    }
    else if(err == LFS_ERR_NOENT) {
        ret = -1;
    }
    fs_dir_close(fs_dir);

    return ret;
}

void cmds_ls(int argc, char *argv[])
{
    int len = 0;
    int ret;
    int is_show_all = false;
    char path[PATH_LEN] = {0};

    if (argc > 3) {
        cmds_ls_help();
        return ;
    }

    if (argc == 1) {
        len = 2;
        path[0] = '.';
    }
    else if(argc == 2) {
        if(strncmp(argv[1], "--all", 5) == 0 && strlen(argv[1]) == 5) {
            is_show_all = true;
            len = 2;
            path[0] = '.';
        }
        else {
            len = strlen(argv[1]) + 1;
            if(len <= PATH_LEN){
                memcpy(path, argv[1], len);
            }
        }
    }    
    else{
        if(strncmp(argv[2], "--all", 5) == 0 && strlen(argv[2]) == 5) {
            is_show_all = true;
            len = strlen(argv[1]) + 1;
            if(len <= PATH_LEN){
                memcpy(path, argv[1], len);
            }
        }
        else if(strncmp(argv[1], "--all", 5) == 0 && strlen(argv[1]) == 5) {
            is_show_all = true;
            len = strlen(argv[2]) + 1;
            if(len <= PATH_LEN){
                memcpy(path, argv[2], len);
            }
        }
        else {
            cmds_ls_help();
            return ;
        }
    }

    Shell *shell = shellGetCurrent();
    char *pcWriteBuffer = pvPortMalloc(PRINT_BUFFER);
    if (pcWriteBuffer == NULL) {
        shellPrint(shell, "No enough memory\n");
    }
    memset(pcWriteBuffer, 0, PRINT_BUFFER);

    ret = ls_dir(pcWriteBuffer, path, len, is_show_all);
    
    if(ret == -1) {
        shellPrint(shell, "ls: %s: No such file or directory\n", path);
    }
    else if(ret == -2) {
        shellPrint(shell, "ls: %s: file or dir name is too long\n", path);
    }
    else if(ret == -3) {
        shellPrint(shell, "ls: %s: read file or dir error\n", path);
    }
    shellWriteString(shell, pcWriteBuffer);

    vPortFree(pcWriteBuffer);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
ls, cmds_ls, ls);

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

static void cmds_echo_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: echo [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - data                           : string, data to be displayed\r\n"
                "      - data1 data2 ... > filename     : echo data to file\r\n"
                "      - data1 data2 ... >> filename    : append data to file\r\n"
            );
}

void cmds_echo(int argc, char *argv[])
{
    char filename[PATH_LEN];
    fs_file_t *fp = NULL;
    int datalength;
    int i;
    int offset = 0;
    char * g_path = fs_get_curr_gpath();

    if(argc < 2){
        cmds_echo_help();
        return ;
    }

    Shell *shell = shellGetCurrent();

    sprintf(filename, "%s%s%s", g_path, "/", argv[argc-1]);
    if(strncmp(argv[argc-2], ">", 1) == 0 && strlen(argv[argc-2]) == 1){
        fp = fs_file_open(filename, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
        if(fp == NULL){
            LOGE(__func__, "open file %s error!", filename);
            return ;
        }
        for(i = 1;i < argc-2; ++i){
            datalength = strlen(argv[i]);
            fs_file_seek(fp, offset, FS_SEEK_SET);
            if(fs_file_write(fp, argv[i], datalength) != datalength){
                LOGE(__func__, "write data to file error!");
                goto CMDEND;
            }
            offset += datalength;
            fs_file_seek(fp, offset, FS_SEEK_SET);
            if(i != argc-3){
                if(fs_file_write(fp, " ", 1) != 1){
                    LOGE(__func__, "write data to file error!");
                    goto CMDEND;
                };
                offset += 1;
            }
        }
        
    }
    else if(strncmp(argv[argc-2], ">>", 2) == 0 && strlen(argv[argc-2]) == 2){
        fp = fs_file_open(filename, FS_O_RDWR | FS_O_APPEND);
        if (fp == NULL){
            LOGE(__func__, "open file %s error!", filename);
            return ;
        }
        
        fs_file_seek(fp, 0, FS_SEEK_END);
        if(fs_file_write(fp, "\r\n", 2) != 2){
            LOGE(__func__, "write data to file error!");
            goto CMDEND;
        }
    
        for(i = 1;i < argc-2; ++i){
            datalength = strlen(argv[i]);
            if(fs_file_write(fp, argv[i], datalength) != datalength){
                LOGE(__func__, "write data to file error!");
                goto CMDEND;
            }
            if(i != argc-3){
                if(fs_file_write(fp, " ", 1) != 1){
                    LOGE(__func__, "write data to file error!");
                    goto CMDEND;
                }
            }
        }
    }
    else {
        for(i = 1;i < argc; ++i){
            shellPrint(shell, argv[i]);
            if(i != argc-1){
                shellPrint(shell, " ");
            }
        }
        return ;
    }

CMDEND:
    if(fp != NULL){
        fs_file_close(fp);
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
echo, cmds_echo, write data to file);

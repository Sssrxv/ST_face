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

static void cmds_cd_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: cd [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - dir: space | . | .. | string\r\n"
            );
}

static int get_fullpath(const char * inpath, char *fullpath)
{
    char * filename = NULL;
    char * pos = NULL;
    const char delim[2] = {"/"};
    int len;
    char path[PATH_LEN] = {0};

    /* we may use inpath again in print, so save it */
    memcpy(path, inpath, PATH_LEN);
    /* absolute path , set fullpath / symbol. 
        otherwise, fullpath is g_path */
    if(strncmp(path, "/", 1) == 0) {
        memset(fullpath, 0, PATH_LEN);
        fullpath[0] = '/';
    }    

    int offset = strlen(fullpath);
    filename = strtok(path, delim);  
    while (filename != NULL)
    {
        if(strncmp(filename, ".", 1) == 0 && strlen(filename) == 1) {
            //do nothing
        }
        else if(strncmp(filename, "..", 2) == 0 && strlen(filename) == 2) {
            pos = strrchr(fullpath, '/');
            if(pos != NULL) {
                len = strlen(pos);
                memset(pos, 0, len);
                offset -= len;
                if(offset < 0) {
                    /** some unknown error happen, reset 
                     *  fullpath to avoid any mem problem 
                     */
                    memset(fullpath, 0, PATH_LEN);
                    fullpath[0] = '/';
                    offset = 1;
                }
            }
            if(strlen(fullpath) == 0) {
                memset(fullpath, 0, PATH_LEN);
                fullpath[0] = '/';
                offset = 1;
            }
        }
        else {
            if(offset != 1) {
                fullpath[offset] = '/';
                offset += 1;
            }
            len = strlen(filename);
            memcpy(fullpath + offset, filename, len);
            offset += len;
        }
        filename = strtok(NULL, delim);
    }

    fullpath[0] = '/';
    memset(fullpath + offset, 0, PATH_LEN - offset);

    return 0;
}

static int cd_dir(const char* path, int len)
{
    fs_dir_t * fs_dir = NULL;
    char fullpath[PATH_LEN] = {0};
    int err;
    int ret = 0;
    char * g_path = fs_get_curr_gpath();

    /* absolute path */
    if (strncmp(path, "/", 1) == 0) {
        if(len > PATH_LEN) {
            ret = -2;
            return ret;
        }
    }
    /* relative path */
    else {
        if(strlen(g_path) + len > PATH_LEN) {
            ret = -2;
            return ret;
        }
    }

    memcpy(fullpath, g_path, strlen(g_path) + 1);
    get_fullpath(path, fullpath);

    fs_dir = fs_dir_open(fullpath, &err);
    if (err == LFS_ERR_NOENT) {
        ret = -1;
    }
    else if(err == LFS_ERR_OK) {
        memcpy(g_path, fullpath, strlen(fullpath) + 1);
    }
    else if(err == LFS_ERR_NOTDIR) {
        ret = -3;
    }
    
    fs_dir_close(fs_dir);

    return ret;
}

void cmds_cd(int argc, char *argv[])
{
    int ret = 0;

    if (argc > 2) {
        cmds_cd_help();
        return ;
    }

    if(argc == 1) {
        ret = cd_dir("/", 2);
    }
    else {
        ret = cd_dir(argv[1], strlen(argv[1]) + 1);
    }

    Shell *shell = shellGetCurrent();
        
    if(ret == -1) {
        shellPrint(shell, "cd: %s: No such file or directory\n", argv[1]);
    }
    else if(ret == -2) {
        shellPrint(shell, "cd: %s: file or dir name is too long\n", argv[1]);
    }
    else if(ret == -3) {
        shellPrint(shell, "cd: %s: Not a directory\n", argv[1]);
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
cd, cmds_cd, cd);

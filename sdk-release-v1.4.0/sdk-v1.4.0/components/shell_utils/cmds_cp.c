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

#define CP_BLOCK_LEN       2048

static void cmds_cp_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: cp [ARGS]\r\n"
                "   [ARGS]: \r\n"
                "       source           : source file to copy from\r\n"
                "       destination      : destination file or directory to copy to\r\n"
                );
}


static int copy_file(const char* src_filename, char * des_filename)
{
    int buflen;
    int offset = 0;    
    int fsize;

    Shell *shell = shellGetCurrent();

    fs_file_t *src_fp = fs_file_open(src_filename, FS_O_RDONLY);
    if (src_fp == NULL)
    {
        shellPrint(shell, "cp: source file do not exist\n");
        return -1;
    }

    fs_file_t *des_fp = fs_file_open(des_filename, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
    if(des_fp == NULL){
        fs_file_close(src_fp);
        shellPrint(shell, "open destination file %s error!\n");
        return -1;
    }

    char *pcWriteBuffer = pvPortMalloc(CP_BLOCK_LEN);
    if (pcWriteBuffer == NULL)
    {
        fs_file_close(src_fp);
        fs_file_close(des_fp);

        shellPrint(shell, "No enough memory\n");
        return -1;
    }

    fsize = fs_file_size(src_fp);

    do
    {
        memset(pcWriteBuffer, 0, CP_BLOCK_LEN);
        fs_file_seek(src_fp, offset, FS_SEEK_SET);
        fs_file_seek(des_fp, offset, FS_SEEK_SET);

        if (fsize - offset >= CP_BLOCK_LEN) 
            buflen = CP_BLOCK_LEN;
        else
            buflen = fsize - offset;

        fs_file_read(src_fp, pcWriteBuffer, buflen);
        if(fs_file_write(des_fp, pcWriteBuffer, buflen) != buflen){
            shellPrint(shell, "write destination file error!\n");
            break;
        }
        offset += buflen;

    } while (offset < fsize);

    fs_file_close(src_fp);
    fs_file_close(des_fp);
    vPortFree(pcWriteBuffer);
    return 0;
}

void cmds_cp(int argc, char *argv[])
{
    int err;
    //int ret;
    char src_filename[PATH_LEN] = {0};
    char des_filename[PATH_LEN] = {0};
    int len;
    char * g_path = fs_get_curr_gpath();

    if (argc != 3) {
        cmds_cp_help();
        return ;
    }

    Shell *shell = shellGetCurrent();

    //source
    len = strlen(argv[1]) + 1;
    if(len > PATH_LEN) {
        shellPrint(shell, "cp: source filename too long.\n");
        return;
    }

    if (strncmp(argv[1], "/", 1) == 0)
        memcpy(src_filename, argv[1], len);
    else
        sprintf(src_filename, "%s%s%s", g_path, "/", argv[1]);

    fs_file_t *src_fp = fs_file_open(src_filename, FS_O_RDONLY);
    if (src_fp == NULL)
    {
        shellPrint(shell, "cp: source file do not exist\n");
        return;
    }
    fs_file_close(src_fp);

    // destination
    len = strlen(argv[2]) + 1;
    if(len > PATH_LEN) {
        shellPrint(shell, "cp: destination filename/directory too long.\n");
        return;
    }

    if (strncmp(argv[2], "/", 1) == 0)
        memcpy(des_filename, argv[2], len);
    else
        sprintf(des_filename, "%s%s%s", g_path, "/", argv[2]);

    fs_dir_t * fs_dir = fs_dir_open(des_filename, &err);
    if (err == LFS_ERR_OK) {
        memset(des_filename, 0, PATH_LEN);
        sprintf(des_filename, "%s%s%s%s%s", g_path, "/", argv[2], "/", argv[1]);
    }
    fs_dir_close(fs_dir);

    fs_file_t *des_fp = fs_file_open(des_filename, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
    if(des_fp == NULL){
        shellPrint(shell, "open destination file %s error!\n");
        return;
    }
    fs_file_close(des_fp);

    copy_file(src_filename, des_filename);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
cp, cmds_cp, cp);

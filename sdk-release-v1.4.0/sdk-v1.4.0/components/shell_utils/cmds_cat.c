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
#include <stdio.h>
#include <string.h>

#define CAT_BLOCK_LEN       2048

static void cmds_cat_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: cat [OPTIONS] [ARGS]\r\n"
                "   [OPTIONS]: \r\n"
                "       -h:     display file in hex format\r\n"
                "       [ARGS]: \r\n"
                "           filename: string\r\n"
                );
}


static int get_file_buf(const char* file, char * pcWriteBuffer, int * offset,  int * buflen)
{
    char filename[PATH_LEN];
    int fsize;
    char * g_path = fs_get_curr_gpath();

    sprintf(filename, "%s%s%s", g_path, "/", file);
    fs_file_t *fp = fs_file_open(filename, FS_O_RDONLY);
    if (fp != NULL)
    {
        fsize = fs_file_size(fp);
        fs_file_seek(fp, *offset, FS_SEEK_SET);
        /* CAT_BLOCK_LEN is 2048 define for cat cmd, however, 
            shellWriteString func only check '\0', so we must confirm
            pcWriteBuffer[2047] is 0 */
        if (fsize - *offset > CAT_BLOCK_LEN - 1) {
            fs_file_read(fp, pcWriteBuffer, CAT_BLOCK_LEN - 1);
            *buflen = CAT_BLOCK_LEN - 1;
            *offset += CAT_BLOCK_LEN - 1;
        }
        else {
            fs_file_read(fp, pcWriteBuffer, fsize - *offset);
            *buflen = fsize - *offset;
            *offset += fsize - *offset;
        }
        
        fs_file_close(fp);
        if (*offset >= fsize) {
            return 0;
        }
        else {
            return 1;
        }
    }

    return -1;
}

void cmds_cat(int argc, char *argv[])
{
    int ret;
    int buflen;
    int offset = 0;
    char data;
    uint8_t is_hex_show = 0;
    uint8_t is_legal_input = 0;
    char filename[PATH_LEN] = {0};
    int len;

    if (argc != 2 && argc != 3) {
        cmds_cat_help();
        return ;
    }

    Shell *shell = shellGetCurrent();

    if(argc == 2) {
        len = strlen(argv[1]) + 1;
        if(len > PATH_LEN) {
            shellPrint(shell, "cat: %s:filename too long\n", argv[1]);
            return ;
        }
        memcpy(filename, argv[1], len);
    }
    if(argc == 3){
        if(strncmp(argv[1], "-h", 2) && strncmp(argv[2], "-h", 2)){
            cmds_cat_help();
            return ;
        }
        
        if((strncmp(argv[1], "-h", 2) == 0) && (strlen(argv[1]) == 2)) {
            len = strlen(argv[2]) + 1;
            if(len > PATH_LEN) {
                shellPrint(shell, "cat: %s:filename too long\n", argv[2]);
                return ;
            }
            memcpy(filename, argv[2], len);
        }
        else if((strncmp(argv[2], "-h", 2) == 0) && (strlen(argv[2]) == 2)) {
            len = strlen(argv[1]) + 1;
            if(len > PATH_LEN) {
                shellPrint(shell, "cat: %s:filename too long\n", argv[1]);
                return ;
            }
            memcpy(filename, argv[1], len);
        }
        else {
            cmds_cat_help();
            return ;
        }
        is_hex_show = 1;
    }

    char *pcWriteBuffer = pvPortMalloc(CAT_BLOCK_LEN);
    if (pcWriteBuffer == NULL)
    {
        shellPrint(shell, "No enough memory\n");
    }

    do
    {
        memset(pcWriteBuffer, 0, CAT_BLOCK_LEN);
        ret = get_file_buf(filename, pcWriteBuffer, &offset, &buflen);
        if(ret >= 0){
            if(is_hex_show){
                shellWriteHex(shell, pcWriteBuffer, buflen);
            }
            else{
                shellWriteString(shell, pcWriteBuffer);
            }
            if(ret > 0){
                shellPrint(shell, "\r\n(press space to continue and q to quit)\n");
            }
        }
        else
        {
            shellPrint(shell, "cat: No such file or directory\n", argv[1]);
            goto CATEND;
        }
        
        while (!is_legal_input && ret > 0){
            if(shell->read && shell->read(&data) == 0){
                switch(data){
                    case 'q':
                    case ' ':
                        is_legal_input = 1;
                        break;
                    default:
                        break;
                }
            }
            usleep(100 * 1000);
        }
        
        is_legal_input = 0;
        if('q' == data){
            break;
        }
        else if(' ' == data){
            continue;
        }
    } while(ret > 0);

CATEND:  

    vPortFree(pcWriteBuffer);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
cat, cmds_cat, cat);

#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"

#include "uart_puts.h"
#include "os_log.h"

static void log_cfg_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: log_cfg [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - uart    : Enable UART puts, ARGS: [uart num] [baud rate] \r\n"
                "      - rtt     : Enable SEGGER RTT,  ARGS: [RTT MODE]\r\n"
                "      - usb     : Enable USB ACM SHELL, ARGS: [None]\r\n"
            );      
}


void cmds_log_cfg(int argc, char *argv[]) 
{
    //uart_puts_config_t uart_cfg = UART_PUTS_CFG_INITIALIZER;
    if (argc < 2) {
        log_cfg_help();
        return ;
    }
 
    if (strncmp(argv[1], "uart", 4) == 0) {
        if (argc != 4) {
            log_cfg_help();
            return ;
        }
        LOGD(__func__, "Switch to UART log mode...");
        int uart_dev = strtol(argv[2], NULL, 0);
        int baudr    = strtol(argv[3], NULL, 0);
        os_log_set_uart_dev_baudr(uart_dev, baudr);
        os_log_set_mode(OS_LOG_UART_MODE);
    }
    else if (strncmp(argv[1], "rtt", 3) == 0) {
        if (argc != 3) {
            log_cfg_help();
            return ;
        }
        LOGD(__func__, "Switch to RTT log mode...");
        int rtt_mode = strtol(argv[2], NULL, 0);
        os_log_set_rtt_mode(rtt_mode);
        os_log_set_mode(OS_LOG_RTT_MODE);
    }
    else if (strncmp(argv[1], "usb", 3) == 0) {
        LOGD(__func__, "Switch to USB log mode...");
        if (argc != 2) {
            log_cfg_help();
            return ;
        }
        os_log_set_mode(OS_LOG_USB_MODE);
    }
    else {
        log_cfg_help();
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
log_cfg, cmds_log_cfg, config log output);

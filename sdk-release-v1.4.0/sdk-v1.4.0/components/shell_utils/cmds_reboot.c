#include <stdlib.h>
#include "shell.h"
#include "os_shell_app.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"
#include "aiva_scu_wdt.h"

static void cmds_reboot_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: reboot [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - timeout:  timeout value in ms\r\n"
            );
}

static int config_wdt(int ms)
{
    scu_wdt_stop();
    scu_wdt_init(1000*ms);
    scu_wdt_work_mode(WDT_WATCHDOG_MODE);
    scu_wdt_irq_register(NULL, NULL, WDT_INTR_PRI);
    scu_wdt_start();
    return 0;
}


void cmds_reboot(int argc, char *argv[])
{
    int timeout_ms = 0;

    if (argc > 2) {
        cmds_reboot_help();
        return ;
    }
    
    int shell_origin_priority = os_shell_app_priority_get();
    int shell_cur_priority = (shell_origin_priority + 4) > configMAX_PRIORITIES ? 
                                    configMAX_PRIORITIES : (shell_origin_priority + 4);
    os_shell_app_priority_set(shell_cur_priority);

    if (argc == 2) {
        timeout_ms = strtol(argv[1], NULL, 0);
        if (timeout_ms <= 0) {
            timeout_ms = 1;
        }
    }
    else {
        timeout_ms = 1;
    }

    config_wdt(timeout_ms);

    LOGD(__func__, "reboot...");
    // Wait WDT timeout...
    while(1);
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
reboot, cmds_reboot, reboot);

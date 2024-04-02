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
#include "videoin.h"
#include "videoin_dev.h"

static cis_dev_driver_t *m_cis_drivers[50] = {NULL};

static void sensor_expo_help(void)
{
    Shell *shell = shellGetCurrent();

    shellPrint(shell, "\r\nindex    width   height   name\r\n");
    for (int i = 0; i < sizeof(m_cis_drivers) / sizeof(m_cis_drivers[0]); i++)
    {
        if (m_cis_drivers[i] == NULL)
            continue;

        cis_frame_param_t param;
        m_cis_drivers[i]->get_frame_parameter(m_cis_drivers[i], &param);
        shellPrint(shell, " %2d     %5d    %5d    %s\r\n", i,
        param.width, param.height, m_cis_drivers[i]->name);
    }
    shellPrint(shell, "\r\n");

    shellPrint(shell, "Usage: sensor_expo [ARGS] \r\n"
                "   [ARGS]: \r\n"
                "      - videoin_id: [0~2], int, videoin id\r\n"
                "      - sens_index: int, senor index\r\n"
                "      - again: float, again value of cam\r\n"    
                "      - dgain: float, dgain value of cam\r\n"    
                "      - itime: float, itime value of cam\r\n"                
            );            
}

void sensor_expo(int argc, char *argv[]) 
{
    videoin_id_t id;
    int sens_index, dvpi_sens;
    float again, dgain, itime;

    cis_get_all_dev_driver_list(m_cis_drivers, sizeof(m_cis_drivers) / sizeof(m_cis_drivers[0]));

    if (argc != 6) {
        sensor_expo_help();
        return ;
    }

    id = strtol(argv[1], NULL, 0);
    sens_index = strtol(argv[2], NULL, 0);
    again      = strtof(argv[3], NULL);
    dgain      = strtof(argv[4], NULL);
    itime      = strtof(argv[5], NULL);

    if (sens_index >= sizeof(m_cis_drivers) / sizeof(m_cis_drivers[0])) {
        sensor_expo_help();
        return;
    }

    cis_dev_driver_t *drv = m_cis_drivers[sens_index];
    cis_exposure_t expo;
    expo.again = again;
    expo.dgain = dgain;
    expo.itime = itime;

    if (drv->set_exposure)
    {
        drv->set_exposure(drv, &expo);
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
sensor_expo, sensor_expo, set sensor expo);

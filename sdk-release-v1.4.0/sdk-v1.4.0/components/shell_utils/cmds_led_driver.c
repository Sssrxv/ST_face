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
#include "led_driver.h"
#include "lm3644_2.h"
#include "gpio_led.h"
#include "videoin.h"
#include "udevice.h"

static led_dev_t *led_dev = NULL;

static void led_driver_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: led_driver [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - init    : init led driver, ARGS: [led_type], [i2c num], [gpio_pin] \r\n"
                // "                : [led_type], 0 - VAS_1071, 1 - LM3644, 2 - TLC591  \r\n"
                "                : [led_type], 0 - VAS_1071, 1 - LM3644, 2 - GPIO  \r\n"
                "                : [i2c_num] , 0 ~ 2 \r\n"
                "                : [gpio_pin], 0 ~ 31\r\n"
                "                : [gpio_control], 1: gpio control, 0: sensor strobe control\r\n"
                "      - release : release led driver \r\n"
                "      - enable  : enable led, ARGS: [led] \r\n"
                "                : [led_num],  0 ~ 1 \r\n"
                "      - disable : disable led, ARGS: [led] \r\n"
                "      - suspend : suspend led, ARGS: [led] \r\n"
                "                : [led_num],  0 ~ 1 \r\n"
                "      - resume  : resume led, ARGS: [led] \r\n"
                "                : [led_num],  0 ~ 1 \r\n"
                "      - current : set current of led,  ARGS: [led_interface], [current level]\r\n"
                "                : [interface], 0 ~ 1 \r\n");
    shellPrint(shell, // split help msg since shell buffer is too small to print whe whole msg
		        "                : [current level],   0~30 \r\n"
                "      - currentma : set current of led,  ARGS: [led_interface], [current mA]\r\n"
                "                : [interface], 0 ~ 1 \r\n"
		        "                : [current mA],   current support by the driver ic from min_range to max_range \r\n"
                "      - timeout : set timeout,     ARGS: [value]\r\n"
                "                : [value],    VAS_1071: 2 ~ 128(ms), LM3644: 10 ~ 400(ms) \r\n"
            );
}

void led_driver(int argc, char *argv[]) 
{
    int ret = 0;
    if (argc < 2) {
        led_driver_help();
        return ;
    }
 
    Shell *shell = shellGetCurrent();
    if (strncmp(argv[1], "init", 4) == 0) {
        if (argc != 5 && argc != 6) {
            led_driver_help();
            return ;
        }
        int led_drv_idx = 0;
        int led_drv_type = strtol(argv[2], NULL, 0);
        char *dev_name = NULL;
        /*if (led_drv_type == 0) {
            dev_name = "vas_1071";
        } else */if (led_drv_type == 1) {
            dev_name = "lm3644";
        } else if (led_drv_type == 2) {
            dev_name = "gpio_led";
        } else {
            shellPrint(shell, "invalid led_type");
            return;
        }

        led_dev = led_find_device(dev_name);
        if (led_dev == NULL) {
            shellPrint(shell, "Can not find %s led device!\n", dev_name);
            return;
        }
        
        int i2c_num  = strtol(argv[3], NULL, 0);
        int gpio_pin = strtol(argv[4], NULL, 0);
        int gpio_control = 1;
        if (argc == 6) {
            gpio_control = strtol(argv[5], NULL, 0);
        }
        if (strncmp(dev_name, "lm3644", strlen("lm3644")) == 0) {
            lm3644_led_t *lm3644_led = (lm3644_led_t*)led_dev->priv;
            lm3644_led->i2c_num = i2c_num;
            lm3644_led->power_pin = gpio_pin;
            lm3644_led->use_strobe = gpio_control ? 0 : 1;
        } else if (strncmp(dev_name, "gpio_led", strlen("gpio_led")) == 0) {
            gpio_led_t *gpio_led = (gpio_led_t*)led_dev->priv;
            gpio_led->pin = gpio_pin;
        }

        ret = led_init(led_dev);
        if (ret != 0) {
            shellPrint(shell, "Initialize %s led device failed!\n", dev_name);
            return;
        }
    }
    else if (strncmp(argv[1], "release", 7) == 0) {
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        led_release(led_dev);
    }
    else if (strncmp(argv[1], "enable", 6) == 0) {
        if (argc != 3) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led_num = strtol(argv[2], NULL, 0);
        led_enable(led_dev, CHANNEL_0);
    }    
    else if (strncmp(argv[1], "disable", 7) == 0) {
        if (argc != 3) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led_num = strtol(argv[2], NULL, 0);
        led_disable(led_dev, CHANNEL_0);
    }
    else if (strncmp(argv[1], "suspend", 7) == 0) {
        if (argc != 3) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led_num = strtol(argv[2], NULL, 0);
        led_suspend(led_dev, CHANNEL_0);
    }
    else if (strncmp(argv[1], "resume", 6) == 0) {
        if (argc != 3) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led_num = strtol(argv[2], NULL, 0);
        led_resume(led_dev, CHANNEL_0);
    }
    else if ((strncmp(argv[1], "currentma", 9) == 0) && (strlen(argv[1]) == 9) ) {
        if (argc != 4) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led = strtol(argv[2], NULL, 0);
        float current_value_ma = strtof(argv[3], NULL);
        led_set_current(led_dev, CHANNEL_0, current_value_ma);
    } 
    else if ((strncmp(argv[1], "current", 7) == 0) && (strlen(argv[1]) == 7) ) {
        if (argc != 4) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int led = strtol(argv[2], NULL, 0);
        int value = strtol(argv[3], NULL, 0);
        led_set_current_level(led_dev, CHANNEL_0, value);
    }
    else if (strncmp(argv[1], "timeout", 7) == 0) {
        if (argc != 3) {
            led_driver_help();
            return ;
        }
        if (led_dev == NULL) {
            Shell *shell = shellGetCurrent();
            shellPrint(shell, "Must call init firstly!\n");
            return ;
        }
        int value = strtol(argv[2], NULL, 0);
        led_set_timeout(led_dev, CHANNEL_0, value);
    }
    else {
        led_driver_help();
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
led_driver, led_driver, init and set led_driver);

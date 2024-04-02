#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "os_core.h"
#include "os_shell_app.h"
#include "shell.h"
#include "shell_port.h"
#include "uvc_comm.h"
#include "os_init_app.h"
#include "syslog.h"
#include "aiva_malloc.h"
#include "usb_func.h"
#include "os_log.h"
#include "aiva_sleep.h"
#include "udevice.h"

static pthread_t m_handle;
static bool m_acm_shell_enable = true;
static Shell *m_cur_shell = NULL;

static bool m_init_usb_func = false;

static int init_usb(void)
{
    int usb_inited = usb_device_has_inited();
    shellPrint(m_cur_shell, "usb inited:%d\n", usb_inited);

    if (!usb_device_has_inited()) {
        int ret;
        const char *usb_gadget_name = "uvc_bulk_gadget";

        uvc_param_t uvc_param = {
            .max_payload_size = 512,
            .ep0_desc_sel = 1
        };

        ret = uvc_init(usb_gadget_name, &uvc_param);
        if (ret != 0) {
            shellPrint(m_cur_shell, "usb func init fail\n");
            return -1;
        }

        // ret = device_find_by_id_name(UCLASS_USB_GARGET, usb_gadget_name, &udev);
        // if (ret < 0) {
        //     LOGE(__func__, "find usb gadget device(%s) error!", usb_gadget_name);
        //     return ret;
        // }

        // if (usb_device_init(udev->driver) != 0)
        // {
        //     shellPrint(m_cur_shell, "usb func init fail\n");
        //     return -1;
        // }
        m_init_usb_func = true;
    }
    else {
        m_init_usb_func = false;
    }
    return 0;
}

static int cleanup_usb(void)
{
    int ret = 0;
    if (m_init_usb_func) {
        // ret = usb_device_cleanup();
        ret = uvc_cleanup();
        m_init_usb_func = false;
    }
    return ret;
}

static void *os_acm_shell_app_thread_entry(void *param)
{
    (void)(param);

    if (m_acm_shell_enable)
    {

        init_usb();
        
        sleep(2);
        // FIXME: skip garbage data
        {
            int ret = 0;
            int cnt = 100;
            int bytes = 0;
            uint8_t buffer[1024];
    
            while (cnt--) {
                ret = usb_function_acm_recv_buffer(buffer, &bytes, USB_FUNC_NO_WAIT);
                (void)ret;
                aiva_msleep(1);
            }
        }
        
        os_log_set_mode(OS_LOG_USB_MODE);
        userShellSetMode(USER_SHELL_USB_MODE);
    }
    else {
        os_log_set_mode(OS_LOG_UART_MODE);
        userShellSetMode(USER_SHELL_UART_MODE);
        cleanup_usb();
    }

    return NULL;
}


int os_acm_shell_app_entry(void)
{
    int ret = 0;
    pthread_attr_t attr;

    m_cur_shell = shellGetCurrent();

    port_set_pthread_name("acm_shell_app");
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024);
    ret = pthread_create(&m_handle, &attr, os_acm_shell_app_thread_entry, NULL);
    
    return ret;
}

void os_usb_acm_shell_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: usb_acm_shell [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - enable:  enable usb acm shell,       ARGS: None \r\n"
                "      - disable: disable usb acm shell,      ARGS: None \r\n");
}

void os_usb_acm_shell(int argc, char *argv[])
{
    bool start = false;

    if (argc == 1) {
        // no arguments, enable acm shell by default
        m_acm_shell_enable = true;
        start = true;
    }
    else if (argc == 2) {
        if (strncmp(argv[1], "enable", 6) == 0) {
            m_acm_shell_enable = true;
            start = true;
        }
        else if (strncmp(argv[1], "disable", 7) == 0) {
            m_acm_shell_enable = false;
            start = true;
        }
        else {
            // do nothing
            os_usb_acm_shell_help();
        }
    }
    else {
        // do nothing
        os_usb_acm_shell_help();
    }
        
    if (start) {
        os_acm_shell_app_entry();
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
usb_acm_shell, os_usb_acm_shell, open acm shell to recv commands);

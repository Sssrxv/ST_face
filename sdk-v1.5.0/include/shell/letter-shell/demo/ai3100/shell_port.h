/**
 * @file shell_port.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @Copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include <stdbool.h>
#include "uart.h"
#include "uart_puts.h"
#include "shell.h"

extern Shell shell;

typedef enum _user_shell_mode_t {
    USER_SHELL_UART_MODE = 0,
    USER_SHELL_USB_MODE,
} user_shell_mode_t;

void userShellInit(void);
void userShellSetMode(user_shell_mode_t mode);
user_shell_mode_t userShellGetMode(void);

void userShellSuspend(void);
void userShellResume(void);

bool userShellIsEanbled(void);

#endif

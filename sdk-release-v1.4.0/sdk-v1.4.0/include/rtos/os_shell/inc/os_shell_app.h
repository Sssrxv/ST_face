#ifndef __OS_SHELL_APP_H__
#define __OS_SHELL_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int os_shell_app_priority_set(int priority);
int os_shell_app_priority_get(void);
int os_shell_app_entry(void);
int parse_shell_string(char *string, uint8_t string_len);

#ifdef __cplusplus
}
#endif 

#endif // __OS_SHELL_APP_H__

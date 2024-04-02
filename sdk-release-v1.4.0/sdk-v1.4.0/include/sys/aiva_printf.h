#ifndef _AIVA_PRINTF_
#define _AIVA_PRINTF_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_LOG_ENABLE
int aiva_vsprintf(char *buf, const char *fmt, va_list args);
int aiva_sprintf(char *buf, const char *fmt, ...);
int aiva_printf(const char *fmt, ...);
#else
#define aiva_vsprintf
#define aiva_sprintf
#define aiva_printf
#endif // CONFIG_LOG_ENABLE

void aiva_printf_enable(void);
void aiva_printf_disable(void);
int aiva_printf_is_enabled(void);

#ifdef __cplusplus
}
#endif
#endif

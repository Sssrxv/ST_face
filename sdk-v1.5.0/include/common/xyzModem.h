#ifndef _XYZMODEM_H_
#define _XYZMODEM_H_

#include <stdbool.h>
#include "aiva_printf.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define xyzModem_xmodem 1
#define xyzModem_ymodem 2
/* Don't define this until the protocol support is in place */
/*#define xyzModem_zmodem 3 */

#define xyzModem_access   -1
#define xyzModem_noZmodem -2
#define xyzModem_timeout  -3
#define xyzModem_eof      -4
#define xyzModem_cancel   -5
#define xyzModem_frame    -6
#define xyzModem_cksum    -7
#define xyzModem_sequence -8

#define xyzModem_close 1
#define xyzModem_abort 2


#define CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT
#define CYGACC_CALL_IF_SET_CONSOLE_COMM(x)

#define diag_vprintf vprintf
// AIVA porting
// #define diag_printf printf
#define diag_printf aiva_printf
#define diag_vsprintf vsprintf

//#define CYGACC_CALL_IF_DELAY_US(x) udelay(x)
#define CYGACC_CALL_IF_DELAY_US(x) aiva_usleep(x)

#define FILENAME_MAX        (256)

typedef enum _modem_chan_t {
    MODEM_CHAN_UART = 0,
    MODEM_CHAN_USB_ACM,
} modem_chan_t;

typedef struct {
    //char *filename;
    char  filename[FILENAME_MAX];
    int   mode;
    int   chan;             // user chan to choose uart or usb acm
    int   uart_num;
    int   uart_baudr;
} connection_info_t;



int   xyzModem_stream_open(connection_info_t *info, int *err);
void  xyzModem_stream_close(int *err);
void  xyzModem_stream_terminate(bool method, int (*getc)(void));
int   xyzModem_stream_read(char *buf, int size, int *err);
char *xyzModem_error(int err);
unsigned long xyzModem_get_file_length(void);
int ymodem_transfer(connection_info_t * info, uint8_t *file_buf, uint32_t file_len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _XYZMODEM_H_ */

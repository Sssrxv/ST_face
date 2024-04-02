#ifndef __LIB_SN_H__
#define __LIB_SN_H__

#include <stdint.h>
#include "flash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sn_t {
    char sn_data[32 + 1]; // +1 for '\0' end of string
} board_sn_t;

int get_board_sn(board_sn_t *sn);
int get_board_sn_from_file(board_sn_t *sn);

#ifdef __cplusplus
}
#endif

#endif //__LIB_SN_H__
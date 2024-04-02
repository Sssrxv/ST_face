#ifndef _XNN_CFG_UTILS_H
#define _XNN_CFG_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if USE_XFILE_MODEL
void* load_xpack_file(void);
#endif

#ifdef __cplusplus
}
#endif

#endif

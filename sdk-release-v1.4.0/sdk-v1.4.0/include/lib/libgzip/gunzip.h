#ifndef __GUNZIP_H__
#define __GUNZIP_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int gunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp);

#ifdef __cplusplus
}
#endif

#endif //__GUNZIP_H__

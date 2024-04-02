#ifndef __GZIP_H__
#define __GZIP_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int gzip(void *dst, unsigned long *lenp,
		unsigned char *src, unsigned long srclen);

#ifdef __cplusplus
}
#endif

#endif //__GZIP_H__

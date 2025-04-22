#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t  crc32(uint32_t crc, const uint8_t *p, uint32_t len);
uint32_t  crc32_no_comp(uint32_t crc, const uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __CRC32_H__

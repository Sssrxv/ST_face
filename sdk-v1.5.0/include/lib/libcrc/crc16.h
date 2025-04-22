#ifndef __CRC16_H__
#define __CRC16_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t crc16_ccitt(uint16_t cksum, const unsigned char *buf, int len);
//void crc16_ccitt_wd_buf(const uint8_t *in, uint len, uint8_t *out, uint chunk_sz);
//
void crc_update(uint16_t* crc, uint8_t ch, bool first);
void crc_update_buffer(uint16_t* crc, const uint8_t* buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __CRC16_H__

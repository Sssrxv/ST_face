#ifndef __LIB_VDE_H__
#define __LIB_VDE_H__

#include "eeprom.h"
#include "frame_mgr.h"
#include "rectify.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EERPOM_MAX_SIZE                 256

typedef struct _eeprom_vde_hdr_t {
    uint16_t magic;         /* magic number: */
    uint16_t length;        /* data length in bytes */
    uint16_t crc16;         /* CRC-16 for ID and data: x^16+x^12+x^5+1 */
    uint64_t id;            /* camera module ID */
    uint8_t  data[0];       /* data pointer */
} __attribute__((packed)) eeprom_vde_hdr_t;

typedef struct _eeprom_vde_param_t {
    eeprom_vde_hdr_t header;
    vde_cam_param_t  cam_param_l;
    vde_cam_param_t  cam_param_r;
    vde_q_param_t    cam_param_q;
} __attribute__((packed)) eeprom_vde_param_t;

int  read_eeprom_vde_param(eeprom_ctrl *eeprom, eeprom_vde_param_t *data_ptr);
void dump_eeprom_vde_param(eeprom_vde_param_t *data_ptr);
int  check_eeprom_vde_param(eeprom_vde_param_t *data_ptr);

int  read_file_vde_param(const char *filename, vde_cam_param_t *cam_l, vde_cam_param_t *cam_r, vde_q_param_t *Q);

#ifdef __cplusplus
}
#endif

#endif // __LIB_VDE_H__

#ifndef _PACK_XNN_FILES_DEF_H
#define _PACK_XNN_FILES_DEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static const unsigned int XNN_WGT_START_ADDR = 0x84000000;
static const uint16_t XNN_WGT_PACK_HEADER_MAGIC = 0xA55A;
static char XNN_WGT_FLASH_PART_NAME[] = "xnn_wgt";

static const unsigned int XNN_XPARAM_XMODEL_START_ADDR = 0x84700000;
static const uint16_t XNN_XPARAM_XMODEL_PACK_HEADER_MAGIC = 0x5AA5;
static char XNN_XFILES_FLASH_PART_NAME[] = "xnn_xfiles";

// TODO: add magic to the end of file to double check file is not croped
// TODO: handle endian different
typedef struct __attribute__((__packed__)) _xnn_wgt_pack_header
{
    uint16_t   magic;
    uint32_t   length;
    uint32_t   dst_addr;
    uint8_t    padding[6];
} xnn_wgt_pack_header_t;

typedef struct __attribute__((__packed__)) _xnn_xparam_xmodel_dic
{
    char       file_name[58];
    uint16_t   file_len;
    uint32_t   file_addr;
} xnn_xparam_xmodel_dic_t;

typedef struct __attribute__((__packed__)) _xnn_xparam_xmodel_pack_header
{
    uint16_t                magic;
    uint16_t                file_dic_num;
    uint32_t                total_len;
    xnn_xparam_xmodel_dic_t file_dic[0]; // place holder
} xnn_xparam_xmodel_pack_header_t;


#define XNN_XPACK_FILE_MAGIC_NUMBER     (0xff081195)
#define XNN_XPACK_FILE_VERSOIN          (0x01)

typedef struct _xnn_xpack_file_header_t {
    uint32_t magic_number;      // 0xff, 0x08, 0x11, 0x95
    uint32_t filesize;          // the current xpack file size
    uint8_t  version;           // file format version
    uint16_t file_count;        // xfile count in this xpack file
    uint8_t  padding0;          // padding byte
    union
    {
        uint32_t reserver0;
        uint32_t algorithm_version; // algorithm version xx.xx.xxxx 0x00010000 for 0.1.0
    };
    union
    {
        uint32_t reserver1;
        uint32_t xnn_models_crc32; // crc32 for xnn models
    };
    uint32_t reserver2;
    uint32_t reserver3;
} __attribute__((packed, aligned(4))) xnn_xpack_file_header_t;

#ifdef __cplusplus
}
#endif

#endif

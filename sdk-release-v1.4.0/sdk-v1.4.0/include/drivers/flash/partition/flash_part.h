#ifndef __FLASH_PART_H__
#define __FLASH_PART_H__

#include <stdint.h>

/* Flash partition related functions */

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_PART_TBL_SIZE             (1 * 1024UL)

/* NAND flash block size is 128KB, keep the backup part table in the other blk */
#define FLASH_PART_TBL_FIRST_ADDR       (256 * 1024UL)
#define FLASH_PART_TBL_SECOND_ADDR      (FLASH_PART_TBL_FIRST_ADDR + 128 * 1024UL)

#define FLASH_PART_HDR_SIZE             (sizeof(flash_part_hdr_t))
#define FLASH_PART_HDR_CNT              (FLASH_PART_TBL_SIZE / FLASH_PART_HDR_SIZE)
#define FLASH_PART_HDR_NAME_SIZE        (16)

#define FLASH_PART_MAGIC                (0x74726170)

/* Mask flags */
#define MASK_FLAG_NOR_FLASH             (1UL << 0)
#define MASK_FLAG_NAND_FLASH            (1UL << 1)

typedef struct _flash_part_hdr_t {
    uint32_t magic;
    char     name[FLASH_PART_HDR_NAME_SIZE];
    uint32_t offset;
    uint32_t size;
    uint32_t mask_flags;
} __attribute__((packed, aligned(4))) flash_part_hdr_t;

int flash_part_create(char *name, uint32_t offset, uint32_t size, uint32_t mask_flags);
int flash_part_update(char *name, uint32_t offset, uint32_t size, uint32_t mask_flags);
int flash_part_rename_update(char *old_name, char *new_name, uint32_t offset, uint32_t size, uint32_t mask_flags);
int flash_part_get(char *name, uint32_t mask_flags, flash_part_hdr_t *hdr_ptr);
int flash_part_list(uint32_t mask_flags);
int flash_part_delete_all(uint32_t mask_flags);
// NOTE: this function is defined as weak symble, so user can re-define
// this function to override default partition table position
uint32_t flash_part_get_part_addr(int use_second, int idx);

#ifdef __cplusplus
}
#endif


#endif // __FLASH_PART_H__

#ifndef __NAND_COMM_H__
#define __NAND_COMM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _bblk_lut_t {
    char *lut;
    int   max_blk;
} bblk_lut_t;

typedef int (*blk_good_func)(uint16_t blk);

int nand_bblk_lut_init(bblk_lut_t *l, blk_good_func is_blk_good);

int nand_bblk_lut_get(bblk_lut_t *l, int blk);

int nand_bblk_lut_set(bblk_lut_t *l, int blk, int val);


#ifdef __cplusplus
}
#endif

#endif // __NAND_COMM_H__

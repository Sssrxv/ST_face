#ifndef _VTE_DESCRIPTOR_H_
#define _VTE_DESCRIPTOR_H_

#include "vte_ctrl.h"
#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _vte_descriptor_t {
    uint8_t desc[32];
} __attribute__((packed, aligned(4))) vte_descriptor_t;

int vte_descriptor_calc(const frame_buf_t *frame, const fast_pt_hdr_t *fast_points, int points_cnt, vte_descriptor_t *descriptors);

vte_descriptor_t *vte_descriptor_alloc(int points_cnt);
int vte_descriptor_free(vte_descriptor_t *ptr);


#ifdef __cplusplus
}
#endif

#endif

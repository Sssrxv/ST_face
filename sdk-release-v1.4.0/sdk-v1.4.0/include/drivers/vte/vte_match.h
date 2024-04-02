#ifndef _VTE_MATCH_H_
#define _VTE_MATCH_H_

#include "vte_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _vte_match_t {
    float distance;
    int   idx0;
    int   idx1;
} __attribute__((packed, aligned(4))) vte_match_t;

int vte_match_calc(const vte_descriptor_t *descs0, int num0, const vte_descriptor_t *descs1, int num1, vte_match_t **matches, int *match_num);


#ifdef __cplusplus
}
#endif

#endif

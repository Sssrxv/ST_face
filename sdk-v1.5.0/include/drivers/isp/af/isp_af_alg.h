#ifndef _AF_ALG_H_
#define _AF_ALG_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AF_FINE_SEARCH_RANGE  (4)
#define AF_FINE_SEARCH_CNT    (AF_FINE_SEARCH_RANGE + 1) // step is 1 and therefore 5 steps to sweep range 4

typedef enum _af_state_t
{
    AF_STATE_INVALID,
    AF_STATE_STOPPED,
    AF_STATE_RUNNING,
    AF_STATE_TRACKING,
    AF_STATE_SELF_STOPPED,
    AF_STATE_MAX
}af_state_t;

typedef struct _af_alg_t
{
    af_state_t          af_state;
    af_search_state_t   search_state;
    float               d_sharpness;
    float               max_sharpness;
    float               curr_sharpness;
    uint32_t            lens_pos;
    int32_t             min_lens_pos;
    int32_t             max_lens_pos;
    int32_t             step_lens_pos; //Current search step used by the af_alg
    int32_t             min_step_lens_pos; //Min search step that lens can support
    uint32_t            min_lens_range;
    uint32_t            max_lens_range;
    int32_t             max_sharpness_pos;
    int                 fine_search_flag;
    int                 reverse_search_flag;
    int32_t             fine_search_cnt;  
    uint32_t            fine_search_sharpness[AF_FINE_SEARCH_CNT];
    uint32_t            cnt;
}af_alg_t;

int                 af_alg_reset(af_alg_t *af_alg);
int                 af_alg_process_one_frame(af_alg_t *af_alg, float sharpness);

void                af_alg_set_af_state(af_alg_t *af_alg, const af_state_t state);
af_state_t          af_alg_get_state(af_alg_t *af_alg);

void                af_alg_set_lense_range(af_alg_t *af_alg, const lens_range_t lens_range);
void                af_alg_get_lens_range(af_alg_t *af_alg, lens_range_t *lens_range);

uint32_t            af_alg_get_lense_pos(af_alg_t *af_alg);
void                af_alg_set_lense_pos(af_alg_t *af_alg, const uint32_t lens_pos);

void                af_alg_search_init(af_alg_t *af_alg);
int                 af_alg_is_searching(af_alg_t *af_alg);
af_search_state_t   af_alg_get_search_state(af_alg_t *af_alg);

float               af_alg_get_curr_sharpness(af_alg_t *af_alg);

#ifdef __cplusplus
}
#endif




#endif

#ifndef __DVPIN_CTL_H__
#define __DVPIN_CTL_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dvpin_num
{
    DVPI_ID_0 = 0,
    DVPI_ID_MAX,
} dvpi_id_t;

typedef enum dvpin_data_type
{
    DVPIN_FMT_YUYV    = 0UL,
    DVPIN_FMT_RGBA    = 3UL,
    DVPIN_FMT_RAW8    = 4UL,
    DVPIN_FMT_RAW16   = 5UL,
    DVPIN_FMT_RAW32   = 6UL,
    DVPIN_FMT_RAW64   = 7UL,
    DVPI_DATA_TYPE_MAX
} dvpin_data_type_t;

typedef enum dvpin_mode
{
    DVPI_SINGLE_MODE     = 0,
    DVPI_CONTINOUS_MODE,
} dvpin_mode_t;

typedef enum dvpin_vs_pol_type
{
    DVPI_VS_POL_TYPE_LEVEL     = 0,
    DVPI_VS_POL_TYPE_PULSE,
} dvpin_vs_pol_type_t;

typedef struct dvpin_ctl_param {
    dvpin_data_type_t format;
    uint32_t h_start;
    uint32_t v_start;
    uint32_t h_stop;
    uint32_t v_stop;
    uint32_t h_size;
    uint32_t v_size;
    uint32_t vs_pol;
    uint32_t x_stride;
    uint32_t y_stride;
} __attribute__((packed, aligned(4))) dvpin_ctl_param_t;

/**
 * @brief       Initialize DVPIN
 */
//int dvpin_init(dvpi_id_t num, uint32_t y_addr, uint32_t c_addr);
int dvpin_init(dvpi_id_t num, uint32_t y_addr, uint32_t c_addr);

int dvpin_ctl_init(dvpi_id_t num, dvpin_ctl_param_t *p_param, uint32_t y_addr, uint32_t c_addr);

int dvpin_start( dvpi_id_t num, dvpin_mode_t mode);

int dvpin_stop( dvpi_id_t num );

int dvpin_wait_frame_end(dvpi_id_t num);

int dvpin_switch_frame( dvpi_id_t num, uint32_t y_addr, uint32_t c_addr);

int dvpin_cmp_shadow_addr( dvpi_id_t num, uint32_t y_addr, uint32_t c_addr );

typedef int (*dvpin_irq_cb_t)(void *ctx);
int dvpin_irq_register(int irq, uint32_t pri, dvpin_irq_cb_t cb, void *ctx);

void dvpin_clear_miwr_interp_status(dvpi_id_t num);
void dvpin_clear_interp_status(dvpi_id_t num);

int dvpin_miwr_irq_enable(dvpi_id_t dev_n);
int dvpin_miwr_irq_disable(dvpi_id_t dev_n);

void dvpin_set_frame_buf( dvpi_id_t num, uint32_t y_addr, uint32_t c_addr );

int dvpin_check_size_error(dvpi_id_t num);

int dvpin_trigger_sw_leak(dvpi_id_t num);

#ifdef __cplusplus
}
#endif

#endif /* __DVPIN_CTL_H__ */

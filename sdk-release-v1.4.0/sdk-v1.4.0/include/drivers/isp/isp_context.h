#ifndef _ISP_CONTEXT_H_
#define _ISP_CONTEXT_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_ae.h"
#include "isp_af.h"
#include "isp_awb.h"
#include "isp_bnr.h"
#include "isp_dpc.h"
#include "libsync_obj.h"


#ifdef __cplusplus
extern "C" {
#endif


// CPROC
//--------

typedef enum _isp_cproc_chrom_out_range_t
{
    ISP_CPROC_CHROM_OUT_RANGE_BT601,
    ISP_CPROC_CHROM_OUT_RANGE_FULL,
    ISP_CPROC_CHROM_OUT_RANGE_MAX
} isp_cproc_chrom_out_range_t;

typedef enum _isp_cproc_lum_in_range_t
{
    ISP_CPROC_LUM_IN_RANGE_BT601,
    ISP_CPROC_LUM_IN_RANGE_FULL,
    ISP_CPROC_LUM_IN_RANGE_MAX
} isp_cproc_lum_in_range_t;


typedef enum _isp_cproc_lum_out_range_t
{
    ISP_CPROC_LUM_OUT_RANGE_BT601,
    ISP_CPROC_LUM_OUT_RANGE_FULL,
    ISP_CPROC_LUM_OUT_RANGE_MAX
} isp_cproc_lum_out_range_t;

typedef struct _isp_cproc_prop_t
{
    isp_cproc_chrom_out_range_t chrom_out_range;
    isp_cproc_lum_in_range_t    lum_in_range;
    isp_cproc_lum_out_range_t   lum_out_range;
} isp_cproc_prop_t;

typedef struct _isp_cproc_setting_t
{
    int                 enabled;
    isp_cproc_prop_t    prop;
    uint8_t             contrast;
    uint8_t             brightness;
    uint8_t             saturation;
    uint8_t             hue;
} isp_cproc_setting_t;



// GAMMA
//-------
typedef struct _isp_gamma_setting_t
{
    int                     enabled;
    isp_gamma_seg_mode_t    seg_mode;
    isp_gamma_curve_t       curve;
} isp_gamma_setting_t;



//  BLS
//-------

typedef struct _isp_black_level_t
{
    uint16_t bls_value[4];
} isp_black_level_t;

typedef struct _isp_bls_setting_t
{
    int                     enabled;
    isp_black_level_t       black_level;
} isp_bls_setting_t;


typedef struct _isp_degamma_setting_t
{
    int                     enabled;
    isp_degamma_curve_t     curve;
} isp_degamma_setting_t;


// CAC
//--------
typedef struct _isp_cac_setting_t
{
    int                     enabled;
    isp_calibdata_cac_t     cac_cfg;
} isp_cac_setting_t;


// WDR
//--------

typedef struct _isp_wdr_setting_t
{
    int                     enabled;
    isp_wdr_curve_t         curve;
} isp_wdr_setting_t;


// CNR
//--------
typedef struct _isp_cnr_setting_t
{
    int                     enabled;
} isp_cnr_setting_t;

// DEMOSAIC
//-----------
typedef struct _isp_demosaic_setting_t
{
    uint8_t                 thresh;
} isp_demosaic_setting_t;


// ISPFILTER
//-----------
typedef struct _isp_filter_setting_t
{
    int                         enabled;
    int                         dns_level;
    int                         sharp_level;
    isp_calibdata_ispfilter_t   filter_cfg;
} isp_filter_setting_t;


typedef struct _isp_dynamic_setting_t
{
    isp_cproc_setting_t     cproc_setting;
    isp_gamma_setting_t     gamma_setting;
    isp_bls_setting_t       bls_setting;
    isp_degamma_setting_t   degamma_setting;
    isp_cac_setting_t       cac_setting;
    isp_wdr_setting_t       wdr_setting;
    isp_cnr_setting_t       cnr_setting;
    isp_demosaic_setting_t  demosaic_setting;
    isp_filter_setting_t    filter_setting;
} isp_dynamic_setting_t;


typedef struct _isp_context_t
{
    isp_config_param_t      config_param;
    isp_sensor_setting_t    sensor_setting;
    isp_dynamic_setting_t   dynamic_setting;
    isp_dpc_context_t       dpc_context;
    isp_bnr_context_t       bnr_context;
    isp_ae_context_t        ae_context;
    isp_af_context_t        af_context;
    isp_awb_context_t       awb_context;
    sync_obj_t              *isp_intr_sync;
    uint32_t                wait_interrupt_events;
    volatile uint32_t       finished_interrupt_events;
} isp_context_t;



#ifdef __cplusplus
}
#endif


#endif

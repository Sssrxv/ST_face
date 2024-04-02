#ifndef _ISPHW_ISP_ACQ_H_
#define _ISPHW_ISP_ACQ_H_

#include "isp_types.h"
#include "isphw_types.h"


#ifdef __cplusplus
extern "C"
{
#endif



typedef struct _isp_acq_prop_t
{
    //NOTE: The dmaSelection will be set in the isphwMuxXXX API.
    acq_sample_edge_t         sampleEdge;
    acq_hsync_pol_t           hSyncPol;
    acq_vsync_pol_t           vSyncPol;
    acq_bayer_pat_t           bayerPat;
    acq_conv422_t            conv422;
    acq_ccir_seq_t            ccirSeq;
    acq_field_selection_t     fieldSelection;
    acq_field_inv_t           fieldInv;
    acq_input_selection_t     inputSelection;
}isp_acq_prop_t;



int isphw_isp_acq_set_prop(const isp_acq_prop_t *acq_prop);
int isphw_isp_acq_set_window(const isp_window_t *window);
int isphw_isp_acq_set_nrframes(uint32_t frames);





#ifdef __cplusplus
}
#endif





#endif

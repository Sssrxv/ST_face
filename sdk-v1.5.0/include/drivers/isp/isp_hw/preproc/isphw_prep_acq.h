#ifndef _ISPHW_PREP_ACQ_H_
#define _ISPHW_PREP_ACQ_H_

#include "isp_types.h"
#include "isphw_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _prep_acq_prop_t
{
    //NOTE: The dmaSelection will be set in the isphwMuxXXX API.
    acq_hsync_pol_t           hSyncPol;
    acq_vsync_pol_t           vSyncPol;
    acq_bayer_pat_t           bayerPat;
    acq_field_selection_t     fieldSelection;
    acq_field_inv_t           fieldInv;
    acq_input_selection_t     inputSelection;
}prep_acq_prop_t;

int isphw_prep_acq_set_prop(const prep_acq_prop_t *acq_prop);
int isphw_prep_acq_set_window(const isp_window_t *window);





#ifdef __cplusplus
}
#endif





#endif

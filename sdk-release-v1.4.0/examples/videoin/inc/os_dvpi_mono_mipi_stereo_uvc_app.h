#ifndef __OS_DVPI_MONO_MIPI_STEREO_UVC_APP_H__
#define __OS_DVPI_MONO_MIPI_STEREO_UVC_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct os_dvpi_mono_mipi_stereo_param {
    int         videoin_id_dvpi;
    const char *sens_type_dvpi;
    int         videoin_id_l;
    int         videoin_id_r;
    const char *sens_type_mipi;    
    int         fps;
} os_dvpi_mono_mipi_stereo_param_t;

#define OS_DVPI_MONO_MIPI_STEREO_PARAM_INITIALIZER       \
    ( ( (os_dvpi_mono_mipi_stereo_param_t)               \
    {                                            \
        .videoin_id_dvpi   = 0,                  \
        .sens_type_dvpi    = NULL,               \
        .videoin_id_l      = 0,                  \
        .videoin_id_r      = 0,                  \
        .sens_type_mipi    = NULL,               \       
        .fps          = 30,                 \
    }                                       \
    )                                       \
    )

int os_dvpi_mono_mipi_stereo_uvc_app_entry(void);

int os_dvpi_mono_mipi_stereo_uvc_app_exit(void);


#ifdef __cplusplus
}
#endif

#endif //__OS_DVPI_MONO_MIPI_STEREO_UVC_APP_H__

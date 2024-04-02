#ifndef __OS_DVPI_MONO_MIPI_MONO_MIPI_STEREO_UVC_APP_H__
#define __OS_DVPI_MONO_MIPI_MONO_MIPI_STEREO_UVC_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct os_dvpi_mono_mipi_mono_mipi_stereo_param {
    int         videoin_id_dvpi_mono;
    const char *sens_type_dvpi_mono;
    int         videoin_id_mipi_mono;
    const char *sens_type_mipi_mono;    
    int         videoin_id_l;
    int         videoin_id_r;
    const char *sens_type_mipi_stereo;    
    int         fps;
} os_dvpi_mono_mipi_mono_mipi_stereo_param_t;

#define OS_DVPI_MONO_MIPI_MONO_MIPI_STEREO_PARAM_INITIALIZER       \
    ( ( (os_dvpi_mono_mipi_mono_mipi_stereo_param_t)               \
    {                                            \
        .videoin_id_dvpi_mono   = 0,                  \
        .sens_type_dvpi_mono    = NULL,               \
        .videoin_id_l      = 0,                  \
        .videoin_id_r      = 0,                  \
        .sens_type_mipi_stereo    = NULL,               \       
        .fps          = 30,                 \
    }                                       \
    )                                       \
    )

int os_dvpi_mono_mipi_mono_mipi_stereo_app_entry(void);

int os_dvpi_mono_mipi_mono_mipi_stereo_app_exit(void);


#ifdef __cplusplus
}
#endif

#endif //__OS_DVPI_MONO_MIPI_MONO_MIPI_STEREO_UVC_APP_H__

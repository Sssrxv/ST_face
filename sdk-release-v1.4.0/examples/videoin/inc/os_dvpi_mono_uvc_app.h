#ifndef __OS_DVPI_MONO_UVC_APP_H__
#define __OS_DVPI_MONO_UVC_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _os_dvpi_mono_uvc_param_t {
    int         videoin_id;
    const char *sens_type;
    int         factory_mode;
    int         rot_angle;
    int         ep0_desc_sel;
    int         fps;
} os_dvpi_mono_uvc_param_t;

#define OS_DVPI_MONO_UVC_PARAM_INITIALIZER       \
    ( ( (os_dvpi_mono_uvc_param_t)               \
    {                                       \
        .videoin_id   = 0,                  \
        .sens_type    = NULL,               \
        .factory_mode = 0,                  \
        .rot_angle    = 0,                  \
        .ep0_desc_sel = 0,                  \
        .fps          = 30,                 \
    }                                       \
    )                                       \
    )

int os_dvpi_mono_uvc_app_entry(void);

int os_dvpi_mono_uvc_app_exit(void);


#ifdef __cplusplus
}
#endif

#endif //__OS_DVPI_MONO_UVC_APP_H__

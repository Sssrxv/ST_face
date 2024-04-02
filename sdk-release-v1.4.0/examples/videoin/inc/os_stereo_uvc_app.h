#ifndef __OS_STEREO_UVC_APP_H_
#define __OS_STEREO_UVC_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _os_stereo_uvc_param_t {
    int         videoin_id_l;
    int         videoin_id_r;
    const char *sens_type;
    int         rot_angle;
    int         ep0_desc_sel;
    int         fps;
} os_stereo_uvc_param_t;

#define OS_STEREO_UVC_PARAM_INITIALIZER     \
    ( ( (os_stereo_uvc_param_t)             \
    {                                       \
        .videoin_id_l = 0,                  \
        .videoin_id_r = 0,                  \
        .sens_type    = NULL,               \
        .rot_angle    = 0,                  \
        .ep0_desc_sel = 0,                  \
        .fps          = 30,                 \
    }                                       \
    )                                       \
    )

int os_stereo_uvc_app_entry(void);

int os_stereo_uvc_app_exit(void);


#ifdef __cplusplus
}
#endif

#endif //__OS_STEREO_UVC_APP_H_

#include <string.h>
#include "os_mipi_cam_comm.h"

#define ALIGN_SIZE(x, n) ((x + n - 1) & (-n))
#define ALIGN_STRIDE(x) ALIGN_SIZE(x, 32)

static int videoin_frameend_callback(videoin_id_t videoin_id, void *ctx)
{
    (void)videoin_id;
    (void)ctx;

    // NOTE: you can trigger led here
    return 0;
}

videoin_context_t *mipi_mono_camera_init(os_mipi_mono_cam_param_t *param)
{
    configASSERT(param != NULL);

    int ret;
    videoin_id_t id = param->id;
    cis_dev_driver_t *drv = param->drv;
    frame_fmt_t frame_fmt = param->frame_fmt;
    int skip_power_on = param->skip_power_on;
    int enable_trigger_led = param->enable_trigger_led;

    cis_frame_param_t      frame_param;
    cis_interface_param_t param_inf;
    cis_format_t fmt;

    drv->get_frame_parameter(drv, &frame_param);
    drv->get_interface_param(drv, &param_inf);

    fmt = frame_param.format;

    videoin_register_cis_driver(id, drv);

    videoin_config_t config;
    memset(&config, 0, sizeof(videoin_config_t));

    config.camera_num = 1;
    config.mode = VIDEOIN_MODE_CONTINUE;
    config.stereo_mode = FRAME_STEREO_NONE;
    config.skip_power_on = skip_power_on;
    config.capture_config[0].videoin_id = id;
    config.capture_config[0].hoffset = 0;
    config.capture_config[0].voffset = 0;
    config.capture_config[0].hsize = frame_param.width;
    config.capture_config[0].vsize = frame_param.height;
    config.capture_config[0].channels = param_inf.mipi_param.vc_num;
    
    if (frame_fmt != FRAME_FMT_MAX) {
        config.capture_config[0].fmt = FRAME_FMT_RAW8;
        config.capture_config[0].hstride = ALIGN_STRIDE(frame_param.width);
    }
    else {
        if (fmt == CIS_FORMAT_RGB_BAYER8 ) {
            config.capture_config[0].fmt = FRAME_FMT_RAW8;
            config.capture_config[0].hstride = ALIGN_STRIDE(frame_param.width);
        }
        else if (fmt == CIS_FORMAT_RGB_BAYER10) {
            config.capture_config[0].fmt = FRAME_FMT_RAW10;
            config.capture_config[0].hstride = ALIGN_STRIDE(frame_param.width) * 2;
        }
        else if (fmt == CIS_FORMAT_RGB_BAYER12) {
            config.capture_config[0].fmt = FRAME_FMT_RAW12;
            config.capture_config[0].hstride = ALIGN_STRIDE(frame_param.width) * 2;
        }
        else {
            LOGE(__func__, "invalid usb mono capture format: %d", fmt);
            return NULL;
        }
    }

    videoin_context_t *context = videoin_init(&config);
    if (context == NULL) {
        LOGE(__func__, "videoin init failed!");
        return NULL;
    }

    ret = videoin_start_stream(context);
    if (ret != 0) {
        videoin_release(context);
        context = NULL;
        LOGE(__func__, "videoin start stream failed!");
        return NULL;
    }

    if (enable_trigger_led) {
        videoin_register_frameend_callback(id, videoin_frameend_callback, NULL);
    }

    return context;
}

int mipi_mono_camera_uninit(videoin_context_t *ctx)
{
    configASSERT(ctx != NULL);
    
    videoin_stop_stream(ctx);
    videoin_release(ctx);

    return 0;
}


videoin_context_t *mipi_stereo_camera_init(videoin_id_t id_l, videoin_id_t id_r, cis_dev_driver_t *drv_l, cis_dev_driver_t *drv_r)
{
    cis_frame_param_t param;
    int w, h;

    configASSERT(drv_l != NULL);
    configASSERT(drv_r != NULL);

    drv_l->get_frame_parameter(drv_l, &param);

    w = param.width;
    h = param.height;
    
    if (w != param.width || h != param.height) {
        LOGE(__func__, "Stereo cameras must have the same W/H. (%d, %d) -- (%d, %d)",
                w, h, param.width, param.height);
        return NULL;
    }

    videoin_register_cis_driver(id_l, drv_l);
    videoin_register_cis_driver(id_r, drv_r);

    videoin_config_t config = VIDEOIN_CONFIG_INITIALIZER;
    memset(&config, 0, sizeof(videoin_config_t));

    config.camera_num = 2;
    config.mode = VIDEOIN_MODE_CONTINUE;
    config.stereo_mode = FRAME_STEREO_V_CONCAT;
    config.capture_config[0].videoin_id = id_l;
    config.capture_config[0].hoffset = 0;
    config.capture_config[0].voffset = 0;
    config.capture_config[0].hsize = param.width;
    config.capture_config[0].vsize = param.height;
    config.capture_config[0].hstride = ALIGN_STRIDE(param.width);
    config.capture_config[0].channels = 1;
    config.capture_config[0].fmt = FRAME_FMT_RAW8;
    config.capture_config[1].videoin_id = id_r;
    config.capture_config[1].hoffset = 0;
    config.capture_config[1].voffset = 0;
    config.capture_config[1].hsize = param.width;
    config.capture_config[1].vsize = param.height;
    config.capture_config[1].hstride = ALIGN_STRIDE(param.width);
    config.capture_config[1].channels = 1;
    config.capture_config[1].fmt = FRAME_FMT_RAW8;

    videoin_context_t *context = videoin_init(&config);
    if (context == NULL) {
        LOGE(__func__, "videoin init failed!");
        return NULL;
    }

    int ret = videoin_start_stream(context);
    if (ret < 0) {
        videoin_release(context);
        context = NULL;
    }

    videoin_register_frameend_callback(id_l, videoin_frameend_callback, NULL);

    return context;
}

int mipi_stereo_camera_uninit(videoin_context_t *ctx)
{
    configASSERT(ctx != NULL);
    
    videoin_stop_stream(ctx);
    videoin_release(ctx);

    return 0;
}


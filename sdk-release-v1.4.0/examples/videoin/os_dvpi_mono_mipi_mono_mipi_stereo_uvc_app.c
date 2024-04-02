#include "udevice.h"
#include "aiva_sleep.h"
#include "uvc_comm.h"
#include "rotate.h"
#include "mjpeg.h"
#include "inc/os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app.h"
#include "rsz.h"
#include "os_isp_core.h"
#include "os_mipi_cam_comm.h"
#include "os_startup.h"

static char* TAG = "os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app";

static pthread_t m_handle = NULL;

static int m_thread_created = 0;
static videoin_context_t *m_videoin_context_dvpi_mono = NULL;
static videoin_context_t *m_videoin_context_mipi_stereo = NULL;
static videoin_context_t *m_videoin_context_mipi_mono = NULL;

static uvc_fmt_t m_uvc_fmt = UVC_MJPEG;
static int m_uvc_w         = 640;
static int m_uvc_h         = 480;
static bool m_uvc_started  = false;

static os_dvpi_mono_mipi_mono_mipi_stereo_param_t m_param = {
    .videoin_id_dvpi_mono   = VIDEOIN_ID_DVPI0,
    .sens_type_dvpi_mono    = "ov7725dvpi",
    .videoin_id_mipi_mono   = VIDEOIN_ID_MIPI0,
    .sens_type_mipi_mono    = "ov02b",     
    .videoin_id_l = VIDEOIN_ID_MIPI2,
    .videoin_id_r = VIDEOIN_ID_MIPI1,
    .sens_type_mipi_stereo    = "ov02b",    
    .fps          = 30,
};

static void user_start_cb(uvc_fmt_t fmt, int w, int h, int fps)
{
    m_uvc_fmt = fmt;
    m_uvc_w = w;
    m_uvc_h = h;
    m_uvc_started = true;
    LOGI(__func__, "fmt %d, w %d, h %d", fmt, w, h);
}

static void user_stop_cb(void)
{
    m_uvc_started = false;
}

static void *os_dvpi_mipi_uvc_app_thread_entry(void *param)
{
    LOGI(__func__, "Start...");

    frame_buf_t *frame_uvc;

    uint8_t discard_cnt = 0;
    int ret = 0;
    int fps = m_param.fps;
    uint32_t ms_per_frame = 1000 / fps;
    uint32_t prev_time = 0;
    static int count = 0;
    static int toggle = 0; 

    while (m_thread_created)
    {
        videoin_frame_t videoin_frame;

        prev_time = aiva_get_curr_ms();
        
        count++;
        if (count % 30 == 0) {
            switch (toggle) {
                case 0:
                    toggle = 1;
                    break;
                case 1:
                    toggle = 2;
                    break;
                case 2:
                    toggle = 0;
                    break;       
                default:
                    toggle = 0;                             
            }
        } 

        videoin_context_t *context = NULL;
        if (toggle == 0) {
            context = m_videoin_context_dvpi_mono;
        } else if (toggle == 1){
            context = m_videoin_context_mipi_mono;
        } else {
            context = m_videoin_context_mipi_stereo;            
        }

        ret = videoin_get_frame(context, &videoin_frame, VIDEOIN_WAIT_FOREVER);
        if (ret < 0) {
            LOGE(__func__, "videoin_get_frame() error!");
            sleep(1);
            continue;
        }
        frame_buf_t *frame_buf = videoin_frame.frame_buf[0];


        if (m_uvc_started) {
            if ((m_uvc_fmt != UVC_GRAY8) && (m_uvc_fmt != UVC_MJPEG)) {
                LOGE(__func__, "Unsupported format(%d)!", m_uvc_fmt);
                frame_mgr_decr_ref(frame_buf);
                sleep(1);
                continue;
            }

            if (frame_buf->fmt == FRAME_FMT_RAW8) {
                if (frame_buf->width != m_uvc_w || frame_buf->height != m_uvc_h) {
                    frame_buf_t *frame_rsz = rsz_resize(frame_buf, m_uvc_w, m_uvc_h, RSZ_GRAY2GRAY);
                    frame_mgr_decr_ref(frame_buf);
                    frame_buf = frame_rsz;
                }
            }

            if (m_uvc_fmt == UVC_MJPEG) {
                frame_uvc = mjpeg_compress(frame_buf, MJPEG_COMP_LEVEL_HIGH);
                frame_mgr_decr_ref(frame_buf);
            }
            else {
                frame_uvc = frame_buf;
            }

            if (discard_cnt < 5) {
                frame_mgr_decr_ref(frame_uvc);
                ++discard_cnt;
            }
            else {
                /*LOGD(__func__, "0x%x, 0x%x", frame_uvc->data[0][0], frame_uvc->data[0][1]);*/
                uvc_commit_frame(frame_uvc);
            }
        }
        else {
            frame_mgr_decr_ref(frame_buf);
        }
        
        uint32_t remain_ms = 0;
        uint32_t cur_time = aiva_get_curr_ms();

        if (cur_time > prev_time) {
            if (cur_time - prev_time < ms_per_frame) {
                remain_ms = ms_per_frame - (cur_time - prev_time);
            }
        }

        if (remain_ms > 0) {
            usleep(remain_ms * 1000);
        }

        prev_time = cur_time;
    }

    return NULL;
}

int os_dvpi_mono_init(void)
{
    int ret;
    
    videoin_id_t id = m_param.videoin_id_dvpi_mono;
    const char *sens_type = m_param.sens_type_dvpi_mono;

    cis_dev_driver_t *drv;

    drv = cis_find_dev_driver(sens_type);
    if (drv == NULL) {
        LOGE(__func__, "Can't find sensor '%s' ...", sens_type);
		return -1;
    }

    videoin_register_cis_driver(id, drv);

    videoin_config_t config;
    memset(&config, 0, sizeof(videoin_config_t));

    config.camera_num = 1;
    config.mode = VIDEOIN_MODE_CONTINUE;
    config.stereo_mode = FRAME_STEREO_NONE;
    config.capture_config[0].videoin_id = id;
    m_videoin_context_dvpi_mono = videoin_init(&config);
    if (m_videoin_context_dvpi_mono == NULL) {
        LOGE(__func__, "videoin_init dvpi fail");
		return -1;
    }

    ret = videoin_start_stream(m_videoin_context_dvpi_mono);
    if (ret < 0) {
        videoin_release(m_videoin_context_dvpi_mono);
        m_videoin_context_dvpi_mono = NULL;
        LOGE(__func__, "videoin_start_stream failed!");
		return -1;
    }

    return 0;
}

int os_mipi_mono_init(void)
{
    int ret;

    videoin_id_t id = m_param.videoin_id_mipi_mono;
    const char *sens_type = m_param.sens_type_mipi_mono;

    cis_dev_driver_t *drv[3];

    ret = cis_find_dev_driver_list(sens_type, &drv[0], 3);
    if (ret < 0) {
        LOGE(__func__, "Can't find sensor '%s' ...", sens_type);
		return -1;
    }

    // init MIPI and sensor
    os_mipi_mono_cam_param_t mono_cam_param = OS_MIPI_MONO_CAM_PARAM_INITIALIZER;
    mono_cam_param.id = id;
    mono_cam_param.drv = drv[id];
    mono_cam_param.frame_fmt = FRAME_FMT_RAW8;
    mono_cam_param.skip_power_on = 0;
    m_videoin_context_mipi_mono = mipi_mono_camera_init(&mono_cam_param);
    if (m_videoin_context_mipi_mono == NULL) {
        LOGE(__func__, "mipi_mono_camera_init failed!");
		return -1;
    }

    return 0;
}

int os_mipi_stereo_init(void)
{
    int ret;

    videoin_id_t id_l     = m_param.videoin_id_l;
    videoin_id_t id_r     = m_param.videoin_id_r;
    const char *sens_type = m_param.sens_type_mipi_stereo;

    cis_dev_driver_t *drv[3];

    ret = cis_find_dev_driver_list(sens_type, &drv[0], 3);
    if (ret < 0) {
        LOGE(__func__, "Can't find sensor '%s' ...", sens_type);
		return -1;
    }

    // init MIPI and sensor
    m_videoin_context_mipi_stereo = mipi_stereo_camera_init(id_l, id_r, drv[id_l], drv[id_r]);
    if (m_videoin_context_mipi_stereo == NULL) {
        LOGE(__func__, "mipi_stereo_camera_init failed!");
		return -1;
    }    

    return 0;
}

int os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app_entry(void)
{
    int ret;
    pthread_attr_t attr;
    const char *uvc_dev_name = NULL;

    ret = os_mipi_mono_init();
    if (ret < 0) {
        goto fail_out;
    }

    ret = os_mipi_stereo_init();
    if (ret < 0) {
        goto fail_out;
    }

    ret = os_dvpi_mono_init();
    if (ret < 0) {
        goto fail_out;
    }

    m_thread_created = 1;

    uvc_param_t uvc_param = UVC_PARAM_INITIALIZER;
    uvc_param.ep0_desc_sel     = 0;
    uvc_param.max_payload_size = 512;
    uvc_dev_name = "uvc_bulk_gadget";

    // init uvc device
    if (uvc_init(uvc_dev_name, &uvc_param) != 0) {
        LOGE(__func__, "Init uvc dev '%s' error!", uvc_dev_name);
        goto fail_out;
    }

    // register UVC callback
    uvc_register_user_start_cb(user_start_cb);
    uvc_register_user_stop_cb(user_stop_cb);


    port_set_pthread_name("mono_uvc_app");
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 16*1024);

    ret = pthread_create(&m_handle, &attr, os_dvpi_mipi_uvc_app_thread_entry, NULL);
    if (ret < 0) {
        goto fail_out;
    }

    //led_ctrl_start();

    return 0;

fail_out:
    os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app_exit();
    return -1;
}

int os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app_exit(void)
{
    LOGI(__func__, "exit");

    if (m_thread_created) {
        m_thread_created = 0;
        if (m_handle != NULL) {
            LOGD(__func__, "pthread_join...");
            pthread_join(m_handle, NULL);
            m_handle = NULL;
        }
        if (m_videoin_context_dvpi_mono != NULL) {
            videoin_release(m_videoin_context_dvpi_mono);
			m_videoin_context_dvpi_mono = NULL;
        }
        if (m_videoin_context_mipi_mono != NULL) {
            mipi_mono_camera_uninit(m_videoin_context_mipi_mono);
			m_videoin_context_mipi_mono = NULL;
        }
        if (m_videoin_context_mipi_stereo != NULL) {
            mipi_stereo_camera_uninit(m_videoin_context_mipi_stereo);
			m_videoin_context_mipi_stereo = NULL;
        }        
        LOGD(__func__, "uvc_cleanup()...");
        uvc_cleanup();
    }

    return 0;    
}

int main(int argc, char *argv[])
{
    LOGI(TAG, "AIVA DVPI MIPI UVC EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

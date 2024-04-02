#include "os_isp_core.h"
#include "os_mipi_cam_comm.h"
#include "udevice.h"
#include "aiva_sleep.h"
#include "uvc_comm.h"
#include "rotate.h"
#include "mjpeg.h"
#include "inc/os_mono_uvc_app.h"
#include "rsz.h"
#include "lm3644_2.h"
#include "os_startup.h"
#include "os_init_app.h"
#include "os_shell_app.h"

/*#define ISP_CALIB_MODE*/

static char* TAG = "os_mono_uvc_app";

static pthread_t m_handle = NULL;

static int m_thread_created = 0;
static videoin_context_t *m_videoin_context = NULL;

static uvc_fmt_t m_uvc_fmt = UVC_MJPEG;
static int m_uvc_w         = 640;
static int m_uvc_h         = 480;
static bool m_uvc_started  = false;

static os_mono_uvc_param_t m_param = {
    .videoin_id   = VIDEOIN_ID_MIPI2,
    .sens_type    = "ov02b",
    .factory_mode = 0,
    .rot_angle    = 0,
    .ep0_desc_sel = 0,
    .fps          = 30,
};

static void user_start_cb(uvc_fmt_t fmt, int w, int h, int fps)
{
    m_uvc_fmt = fmt;
    m_uvc_w = w;
    m_uvc_h = h;
    m_uvc_started = true;
    LOGI(__func__, "fmt %d, w %d, h %d, fps:%d", fmt, w, h, fps);
}

static void user_stop_cb(void)
{
    m_uvc_started = false;
}

static void *os_mono_uvc_app_thread_entry(void *param)
{
    (void)param;
    LOGI(__func__, "Start...");

    frame_buf_t *frame_uvc;

    uint8_t discard_cnt = 0;
    int ret = 0;
    int fps = m_param.fps;
    uint32_t ms_per_frame = 1000 / fps;
    uint32_t prev_time = 0;

    int rot_angle = m_param.rot_angle;

    while (m_thread_created)
    {
        videoin_frame_t videoin_frame;

        prev_time = aiva_get_curr_ms();
        
        ret = videoin_get_frame(m_videoin_context, &videoin_frame, VIDEOIN_WAIT_FOREVER);
        if (ret < 0) {
            LOGE(__func__, "videoin_get_frame() error!");
            sleep(1);
            continue;
        }
        frame_buf_t *frame_buf = videoin_frame.frame_buf[0];

        /*LOGD(__func__, "get frame_buf 0x%08x", frame_buf);*/

        if (m_uvc_started) {
            if ((m_uvc_fmt != UVC_GRAY8) && (m_uvc_fmt != UVC_MJPEG)) {
                LOGE(__func__, "Unsupported format(%d)!", m_uvc_fmt);
                frame_mgr_decr_ref(frame_buf);
                sleep(1);
                continue;
            }

            if (rot_angle != 0) {
                int rotate_mode = ROT_MOD_CLK_90;
                switch (rot_angle) {
                    case 90:
                        rotate_mode = ROT_MOD_CLK_90;
                        break;
                    case 180:
                        rotate_mode = ROT_MOD_CLK_180;
                        break;
                    case 270:
                        rotate_mode = ROT_MOD_CLK_270;
                        break;
                    default:
                        LOGE(__func__, "Unsupported rot angle %d", rot_angle);
                        break;
                }

                frame_buf_t *frame_rot = rotate_frame(frame_buf, rotate_mode, 0);
                frame_mgr_decr_ref(frame_buf);
                frame_buf = frame_rot;
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

int os_mono_uvc_app_entry(void)
{
    int ret;
    pthread_attr_t attr;
    
    const char *uvc_dev_name = NULL;

    /* Mount file system */
    os_mount_fs();

    /* Start shell */
    os_shell_app_entry();

    if (m_thread_created) {
        return 0;
    }

    videoin_id_t id = m_param.videoin_id;
    const char *sens_type = m_param.sens_type;

    if (m_param.factory_mode) {
       uvc_dev_name = "uvc_isoc_camera";
    }
    else {
       uvc_dev_name = "uvc_bulk_gadget";
    }

    m_thread_created = 1;

    cis_dev_driver_t *drv[3];

    ret = cis_find_dev_driver_list(sens_type, &drv[0], 3);
    if (ret < 0) {
        LOGE(__func__, "Can't find sensor '%s' ...", sens_type);
		goto fail_out;
    }

    // init MIPI and sensor
    os_mipi_mono_cam_param_t mono_cam_param = OS_MIPI_MONO_CAM_PARAM_INITIALIZER;
    mono_cam_param.id = id;
    mono_cam_param.drv = drv[id];
#ifndef ISP_CALIB_MODE
    mono_cam_param.frame_fmt = FRAME_FMT_RAW8;
#else
    mono_cam_param.frame_fmt = FRAME_FMT_MAX;
#endif
    mono_cam_param.skip_power_on = 0;
    m_videoin_context = mipi_mono_camera_init(&mono_cam_param);
    if (m_videoin_context == NULL) {
        goto fail_out;
    }

    uvc_param_t uvc_param = UVC_PARAM_INITIALIZER;

    uvc_param.ep0_desc_sel     = m_param.ep0_desc_sel;
#ifndef ISP_CALIB_MODE
    uvc_param.max_payload_size = 512;
#else
    uvc_param.max_payload_size = 16 * 1024;
#endif

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

    ret = pthread_create(&m_handle, &attr, os_mono_uvc_app_thread_entry, NULL);
    if (ret < 0) {
        goto fail_out;
    }

    return 0;

fail_out:
    os_mono_uvc_app_exit();
    return -1;
}

int os_mono_uvc_app_exit(void)
{
    LOGI(__func__, "exit os_mono_uvc_app() ...");

    if (m_thread_created) {
        m_thread_created = 0;
        if (m_handle != NULL) {
            LOGD(__func__, "pthread_join...");
            pthread_join(m_handle, NULL);
            m_handle = NULL;
        }
        if (m_videoin_context != NULL) {
            LOGD(__func__, "mipi_mono_camera_uninit ...");
            mipi_mono_camera_uninit(m_videoin_context);
			m_videoin_context = NULL;
        }
        LOGD(__func__, "uvc_cleanup()...");
        uvc_cleanup();
    }

    return 0;    
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    LOGI(TAG, "AIVA MIPI MONO UVC EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(os_mono_uvc_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

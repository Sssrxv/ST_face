#include "facelock_sdk_test.h"
#include "facelock_sdk.h"
#include "syslog.h"
#include "aiva_sleep.h"

#include "FreeRTOS_POSIX/pthread.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static facelock_enroll_info_t m_enroll_info = {
    .admin = 0,
    .user_name = {'\0'},
    .face_direction = FACELOCK_FACE_DIRECTION_MIDDLE,
    .enroll_type = 1, // single enroll mode
    .enable_duplicate = 1, // allow duplicated enroll
    .timeout_second = 10,
};

static facelock_enroll_info_t m_interactive_enroll_info = {
    .admin = 0,
    .user_name = {'\0'},
    .face_direction = FACELOCK_FACE_DIRECTION_MIDDLE,
    .enroll_type = 0, // interactive enroll mode
    .enable_duplicate = 1, // allow duplicated enroll
    .timeout_second = 10,
};

static facelock_enroll_result_t m_enroll_result = {
    .user_id_hi = 0xFF,
    .user_id_lo = 0xFF,
    .face_direction = FACELOCK_FACE_DIRECTION_MIDDLE,
};

static facelock_verify_info_t m_verify_info = {
    .pd_rightaway = 0,
    .timeout_second = 10,
};

static facelock_verify_result_t m_verify_result = {
    .user_id_hi = 0xFF,
    .user_id_lo = 0xFF,
    .user_name = {'\0'},
    .admin = 0,
    .unlock_status = 200,
};

typedef enum test_state
{
    TEST_STATE_IDLE = 0,
    TEST_STATE_PENDING_RESULT,
    TEST_STATE_CMD_SUCCUSS,
    TEST_STATE_CMD_FAILED,
} test_state_t;

static test_state_t m_test_state = TEST_STATE_IDLE;
static facelock_result_t m_facelock_result = FACELOCK_RESULT_SUCCESS;

static void enroll_callback(facelock_result_t result, const facelock_enroll_result_t *enroll_result)
{
    m_facelock_result = result;
    if (result == FACELOCK_RESULT_SUCCESS)
    {
        m_test_state = TEST_STATE_CMD_SUCCUSS;
        m_enroll_result = *enroll_result;
    }
    else
    {
        m_test_state = TEST_STATE_CMD_FAILED;
    }
}

static void verify_callback(facelock_result_t result, const facelock_verify_result_t *verify_result)
{
    m_facelock_result = result;
    if (result == FACELOCK_RESULT_SUCCESS)
    {
        m_test_state = TEST_STATE_CMD_SUCCUSS;
        m_verify_result = *verify_result;
    }
    else
    {
        m_test_state = TEST_STATE_CMD_FAILED;
    }
}


static int enroll_face()
{
    int ret = -1;
    LOGI("", "\n\n%s test start.", __func__);
    m_test_state = TEST_STATE_PENDING_RESULT;
    ret = facelock_enroll(&m_enroll_info, enroll_callback);
    if (ret != 0)
    {
        LOGE("", "enroll failed");
        return ret;
    }

    while (m_test_state == TEST_STATE_PENDING_RESULT)
    {
        LOGD("", "wait for enroll result");
        aiva_msleep(1000);
    }

    if (m_facelock_result == FACELOCK_RESULT_SUCCESS)
    {
        uint16_t new_user_id = m_enroll_result.user_id_hi << 8 | m_enroll_result.user_id_lo;
        LOGI("", "enroll success, new user id is:%d", (int)new_user_id);
        ret = 0;
    }
    else
    {
        LOGD("", "enroll failed with result:%d", (int)m_facelock_result);
        ret = -1;
    }

    return ret;
}

static int interactive_enroll_face()
{
    int ret = -1;
    LOGI("", "\n\n%s test start.", __func__);

    ret = facelock_reset_enroll_state();
    LOGI(__func__, "reset enroll state ret:%d", ret);

    int direction[] = {FACELOCK_FACE_DIRECTION_MIDDLE,
                       FACELOCK_FACE_DIRECTION_LEFT,
                       FACELOCK_FACE_DIRECTION_RIGHT,
                       FACELOCK_FACE_DIRECTION_UP,
                       FACELOCK_FACE_DIRECTION_DOWN};

    for (int i = 0; i < 5; i++)
    {
        LOGI("", "interactive enroll direction %d.", direction[i]);
        m_test_state = TEST_STATE_PENDING_RESULT;
        m_interactive_enroll_info.face_direction = direction[i];
        ret = facelock_enroll(&m_interactive_enroll_info, enroll_callback);
        if (ret != 0)
        {
            LOGE("", "enroll failed");
            return ret;
        }

        while (m_test_state == TEST_STATE_PENDING_RESULT)
        {
            LOGD("", "wait for enroll result");
            aiva_msleep(1000);
        }
    }

    if (m_facelock_result == FACELOCK_RESULT_SUCCESS)
    {
        uint16_t new_user_id = m_enroll_result.user_id_hi << 8 | m_enroll_result.user_id_lo;
        LOGI("", "enroll success, new user id is:%d", (int)new_user_id);
        ret = 0;
    }
    else
    {
        LOGD("", "enroll failed with result:%d", (int)m_facelock_result);
        ret = -1;
    }

    return ret;
}

static int verify_face()
{
    int ret = -1;
    LOGI("", "\n\n%s test start.", __func__);
    m_test_state = TEST_STATE_PENDING_RESULT;
    ret = facelock_verify(&m_verify_info, verify_callback);
    if (ret != 0)
    {
        LOGE("", "verify failed");
        return -1;
    }

    while (m_test_state == TEST_STATE_PENDING_RESULT)
    {
        LOGD("", "wait for verify result");
        aiva_msleep(1000);
    }

    if (m_facelock_result == FACELOCK_RESULT_SUCCESS)
    {
        uint16_t user_id = m_verify_result.user_id_hi << 8 | m_verify_result.user_id_lo;
        LOGI("", "verify success, user id is:%d", (int)user_id);
        ret = 0;
    }
    else
    {
        LOGD("", "verify failed with code:%d", (int)m_facelock_result);
        ret = -1;
    }

    return ret;
}


static void enroll_verify_test()
{
    for (int i = 0; i < 10; i++)
    {
        int ret = 0;
        if (i == 0)
        {
            ret = interactive_enroll_face();
        }
        else
        {
            ret = enroll_face();
        }

        if (ret != 0)
        {
            return;
        }
        //
        ret = verify_face();
        if (ret != 0)
        {
            return;
        }
    }
}

static bool should_exit = false;
static void *reset_running_cmd_thread(void *param)
{
    LOGD("", "%s running", __func__);
    while (!should_exit)
    {
        aiva_sleep(1); // sleep to make sure has running cmd
        facelock_result_t result = facelock_reset();
        if (result != FACELOCK_RESULT_SUCCESS)
        {
            LOGE("", "reset facelock failed");
        }
    }
    LOGD("", "%s exit", __func__);

   return NULL;
}

static void reset_test()
{
    // start a thread to cancel current running cmd
    int ret;
    pthread_t handle;

    LOGW("", "\n\n\n%s Will start after 3 seconds.....\n\n\n", __func__);
    aiva_sleep(3);

    ret = pthread_create(&handle, NULL, reset_running_cmd_thread, NULL);
    if (ret < 0)
    {
        LOGE(__func__, "Create thread error!\n");
        return;
    }

    ret = enroll_face();
    if (ret == -1 && m_facelock_result == FACELOCK_RESULT_CANCELLED)
    {
        LOGI("", "enroll been cancelled");
    }
    else
    {
        LOGW("", "you should cover camera to make this test case pass with FACELOCK_RESULT_CANCELLED");
    }
    //
    ret = verify_face();
    if (ret == -1 && m_facelock_result == FACELOCK_RESULT_CANCELLED)
    {
        LOGI("", "verify been cancelled");
    }
    else
    {
        LOGW("", "you should cover camera to make this test case pass with FACELOCK_RESULT_CANCELLED");
    }
    //
    should_exit = true;
    pthread_join(handle, NULL);
    //
    LOGI("", "%s test pass.", __func__);
}

static void delete_user_test()
{
    LOGI("", "\n\n%s test start.", __func__);
    int ret = facelock_delete_all_user();
    if (ret != 0)
    {
        LOGE("", "delete all user failed");
        return;
    }
    //
    ret = verify_face();
    if (ret != 0 && m_facelock_result == FACELOCK_RESULT_FAILED4_UNKNOWNUSER)
    {
        LOGI("", "delete all user test pass.");
    }
    else
    {
        LOGE("", "should not verify success");
    }
    //
    ret = enroll_face();
    if (ret != 0)
    {
        LOGE("", "enroll failed");
        return;
    }
    //
    ret = facelock_delete_user(m_enroll_result.user_id_hi << 8 | m_enroll_result.user_id_lo);
    if (ret != 0)
    {
        LOGE("", "delete user failed");
        return;
    }
    //
    ret = verify_face();
    if (ret != 0 && m_facelock_result == FACELOCK_RESULT_FAILED4_UNKNOWNUSER)
    {
        LOGI("", "delete user test pass");
    }
    else
    {
        LOGE("", "should not verify success");
    }
}

// NOTE: not do time consuming task in this callback, only set some flag and return
// even do not print some debug info as following example
static void notify_callback(facelock_facestate_t facestate, const facelock_face_pose_t *face_pose)
{
    LOGI("", "facestate:%d", facestate);
}

int facelock_sdk_api_mode_test_entry(void)
{
    LOGI("", "%s", __func__);
    facelock_init_params_t init_params = FACELOCK_INIT_PARAMS_INITIALIZER;
    init_params.start_mode = FACELOCK_MODE_API_MODE;
    init_params.mount_fs = 1;
    init_params.start_shell = 1;
    init_params.mode_params.api_mode_params.notify_callback = notify_callback;

    int ret = facelock_init(init_params);
    if (ret != 0)
    {
        LOGE("", "facelock init failed");
        return ret;
    }

    //
    enroll_verify_test();
    //
    reset_test();
    //
    delete_user_test();

    ret = facelock_uninit();
    if (ret != 0)
    {
        LOGE("", "facelock uninit failed");
        return ret;
    }

    return ret;
}

int facelock_sdk_uart_mode_test_entry(void)
{
    LOGI("", "%s", __func__);
    facelock_init_params_t init_params = FACELOCK_INIT_PARAMS_INITIALIZER;
    init_params.start_mode = FACELOCK_MODE_SERIAL_PROTOCOL_MODE;
    init_params.mode_params.serial_protocol_mode_params.communicate_serial_num = 1; // uart1
#if defined(USE_FACELOCK_SDK_TEST)
    init_params.mount_fs = 1;
    init_params.start_shell = 1;
#endif

    int ret = facelock_init(init_params);
    if (ret != 0)
    {
        LOGE("", "facelock init failed");
        return ret;
    }

    return ret;
}

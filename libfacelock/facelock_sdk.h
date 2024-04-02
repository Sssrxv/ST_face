#ifndef __FACELOCK_SDK_H__
#define __FACELOCK_SDK_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_USER_NAME_SIZE 32

typedef enum
{
    FACELOCK_RESULT_SUCCESS                  = 0,   // success
    FACELOCK_RESULT_REJECTED                 = 1,   // module rejected this command
    FACELOCK_RESULT_ABORTED                  = 2,   // algo aborted
    FACELOCK_RESULT_FAILED4_CAMERA           = 4,   // camera open failed
    FACELOCK_RESULT_FAILED4_UNKNOWNREASON    = 5,   // UNKNOWN_ERROR
    FACELOCK_RESULT_FAILED4_INVALIDPARAM     = 6,   // invalid param
    FACELOCK_RESULT_FAILED4_NOMEMORY         = 7,   // no enough memory
    FACELOCK_RESULT_FAILED4_UNKNOWNUSER      = 8,   // Unknown user
    FACELOCK_RESULT_FAILED4_MAXUSER          = 9,   // exceed maximum user number
    FACELOCK_RESULT_FAILED4_FACEENROLLED     = 10,  // this face has been enrolled
    FACELOCK_RESULT_FAILED4_LIVENESSCHECK    = 12,  // liveness check failed
    FACELOCK_RESULT_FAILED4_TIMEOUT          = 13,  // exceed the time limit
    FACELOCK_RESULT_FAILED4_AUTHORIZATION    = 14,  // authorization failed
    FACELOCK_RESULT_FAILED4_CAMERAFOV        = 15,  // camera fov test failed
    FACELOCK_RESULT_FAILED4_CAMERAQUA        = 16,  // camera quality test failed
    FACELOCK_RESULT_FAILED4_CAMERASTRU       = 17,  // camera structure test failed
    FACELOCK_RESULT_FAILED4_BOOT_TIMEOUT     = 18,  // boot up timeout
    FACELOCK_RESULT_FAILED4_READ_FILE        = 19,  // read file failed
    FACELOCK_RESULT_FAILED4_WRITE_FILE       = 20,  // write file failed
    FACELOCK_RESULT_FAILED4_NO_ENCRYPT       = 21,  // encrypt must be set
    //
    FACELOCK_RESULT_CANCELLED                = 22,  // cmd has been cancelled
} facelock_result_t;

typedef enum
{
    FACELOCK_FACE_DIRECTION_UP        =  (1 << 4),    // face up
    FACELOCK_FACE_DIRECTION_DOWN      =  (1 << 3),    // face down
    FACELOCK_FACE_DIRECTION_LEFT      =  (1 << 2),    // face left
    FACELOCK_FACE_DIRECTION_RIGHT     =  (1 << 1),    // face right
    FACELOCK_FACE_DIRECTION_MIDDLE    =  (1 << 0),    // face middle
    FACELOCK_FACE_DIRECTION_UNDEFINE  =  0x00,        // face undefine
} facelock_face_direction_t;

typedef enum
{
    FACELOCK_UVC_STREAM_START = 0,
    FACELOCK_UVC_STREAM_STOP  = 1,
    FACELOCK_UVC_STREAM_INVAL,
} facelock_uvc_state_t;

typedef int (*facelock_uvc_strm_cb_t)(facelock_uvc_state_t state);

typedef struct
{
    uint8_t admin;                      // enroll as admin or not, 1 - admin, 0 - normal user
    uint8_t user_name[MAX_USER_NAME_SIZE];
    uint8_t face_direction;             // facelock_face_direction_t, enroll face of this direction
    uint8_t enroll_type;                // 0 - interactive, 1 - single
    /* 0 - Don't allow duplicated enrollment, but the user name can be duplicated.
     * 1 - Allow duplicated enrollment and the user name can be duplicated.
     * 2 - Allow duplicated enrollment, but the user name can't be duplicated.
     */
    uint8_t enable_duplicate;
    uint8_t timeout_second;             // timeout unit second, default 10s
    uint8_t resv[3];
} facelock_enroll_info_t;

typedef struct
{
    uint8_t user_id_hi;        // high byte of new user id
    uint8_t user_id_lo;        // low byte of new user id
    uint8_t face_direction;    // current direction state of enrolled user
} facelock_enroll_result_t;

typedef struct
{
    uint8_t pd_rightaway;      // power down right away after verifying, ignore
    uint8_t timeout_second;    // timeout in second, default 10s
} facelock_verify_info_t;

typedef struct
{
    uint8_t user_id_hi;                       // high byte of user id
    uint8_t user_id_lo;                       // low byte of user id
    uint8_t user_name[MAX_USER_NAME_SIZE];    // name of this user
    uint8_t admin;                            // user is admin or not
    uint8_t unlock_status;                    // unlock status
} facelock_verify_result_t;

typedef enum
{
    FACELOCK_FACESTATE_NORMAL                           = 0,   // normal state, the face is available
    FACELOCK_FACESTATE_NOFACE                           = 1,   // no face detected
    FACELOCK_FACESTATE_TOOUP                            = 2,   // face is too up side
    FACELOCK_FACESTATE_TOODOWN                          = 3,   // face is too down side
    FACELOCK_FACESTATE_TOOLEFT                          = 4,   // face is too left side
    FACELOCK_FACESTATE_TOORIGHT                         = 5,   // face is too right side
    FACELOCK_FACESTATE_TOOFAR                           = 6,   // face is too far
    FACELOCK_FACESTATE_TOOCLOSE                         = 7,   // face is too near
    FACELOCK_FACESTATE_EYEBROW_OCCLUSION                = 8,   // eyebrow occlusion
    FACELOCK_FACESTATE_EYE_OCCLUSION                    = 9,   // eye occlusion
    FACELOCK_FACESTATE_FACE_OCCLUSION                   = 10,  // face occlusion
    FACELOCK_FACESTATE_DIRECTION_ERROR                  = 11,  // face direction error
    FACELOCK_FACESTATE_EYE_CLOSE_STATUS_OPEN_EYE        = 12,  // eye close time out
    FACELOCK_FACESTATE_EYE_CLOSE_STATUS                 = 13,  // confirm eye close status
    FACELOCK_FACESTATE_EYE_CLOSE_UNKNOW_STATUS          = 14,  // eye close unknow status
} facelock_facestate_t;

typedef struct
{
    float roll;              // roll angle of head
    float pitch;             // pitch angle of head
    float yaw;               // yaw angle of head
} facelock_face_pose_t;

typedef enum
{
    FACELOCK_MODE_SERIAL_PROTOCOL_MODE = 0,    // use sensetime serial protocol
    FACELOCK_MODE_API_MODE,                    // use api
} facelock_start_mode_t;

typedef struct
{
    uint8_t major;   // major version
    uint8_t minor;   // minor version
    uint16_t patch;  // patch version
} facelock_version_t;

/**
 * @brief callback function to notice app current face state during enroll or verify process
 *
 * @param[in] facestate  current facestate, can value of #facelock_facestate_t
 * @param[in] face_pose  current facepose, euler angle of head, ignore now
 *
 * @note  memory pointed by face_pose is invalid after callback return
 */
typedef void (*notify_callback_t)(facelock_facestate_t facestate, const facelock_face_pose_t *face_pose);

/**
 * @brief callback function to notice app the result of enroll process
 * @param[in] result result of enroll, can be one of following value
                     FACELOCK_RESULT_SUCCESS                 enroll success, get new user info at #enroll_result
                     FACELOCK_RESULT_FAILED4_MAXUSER         exceed maximum user number, default maximum user number is 50
                     FACELOCK_RESULT_FAILED4_FACEENROLLED    this face has been enrolled, will return this value only when enroll_info->enable_duplicate == 0
                     FACELOCK_RESULT_FAILED4_LIVENESSCHECK   liveness check failed
                     FACELOCK_RESULT_FAILED4_TIMEOUT         exceed the time limit
                     FACELOCK_RESULT_CANCELLED               cmd has been cancelled
   @param[in] enroll_result newly enrolled user info, only valid when #result is FACELOCK_RESULT_SUCCESS
   @note  memory pointed by enroll_result is invalid after callback return
 */
typedef void (*enroll_callback_t)(facelock_result_t result, const facelock_enroll_result_t *enroll_result);

/**
 * @brief callback function to notice app the result of verfiy process
 * @param[in] result result of verfiy, can be one of following value
                     FACELOCK_RESULT_SUCCESS                 verfiy success, get user_info at #verify_result
                     FACELOCK_RESULT_FAILED4_UNKNOWNUSER     unknown user
                     FACELOCK_RESULT_FAILED4_LIVENESSCHECK   liveness check failed
                     FACELOCK_RESULT_FAILED4_TIMEOUT         exceed the time limit
                     FACELOCK_RESULT_CANCELLED               cmd has been cancelled
   @param[in] verify_result result of verify, only valid when #result is FACELOCK_RESULT_SUCCESS
   @note  memory pointed by verify_result is invalid after callback return
 */
typedef void (*verify_callback_t)(facelock_result_t result, const facelock_verify_result_t *verify_result);

typedef struct facelock_uvc_cfg
{
    int enable_night_vision;           // use ir frame when in dark environment
    uint8_t swith_to_ir_threshold;     // mean luma to switch to ir frame
} facelock_uvc_cfg_t;

#define DEFAULT_ENABLENIGHT_VISION (0)
#define DEFAULT_DARK_THRESHOLD (20)
#define MIN_DARK_THRESHOLD (10)
#define MAX_DARK_THRESHOLD (100)

#define FACELOCK_UVC_PARAMS_INITIALIZER                             \
    (((facelock_uvc_cfg_t){                                         \
            .enable_night_vision = DEFAULT_ENABLENIGHT_VISION,      \
            .swith_to_ir_threshold = DEFAULT_DARK_THRESHOLD,        \
    }))

typedef struct
{
    facelock_start_mode_t start_mode;                // libfacelock start mode, api mode or uart mode
    uint8_t mount_fs;                                // whether mount fs in libfacelock
    uint8_t start_shell;                             // whether start shell in libfacelock
    union
    {
        struct
        {
            uint8_t communicate_serial_num;          // uart num when libfacelock work in uart mode
        } serial_protocol_mode_params;               // parameters for uart work mode
        struct
        {
            notify_callback_t notify_callback;       // notity callback when work in api mode, used to report face state
        } api_mode_params;                           // parameters for api work mode
    } mode_params;
} facelock_init_params_t;

#define FACELOCK_INIT_PARAMS_INITIALIZER                          \
    (((facelock_init_params_t){                                   \
        .start_mode = FACELOCK_MODE_API_MODE,                     \
        .mount_fs = 0,                                            \
        .start_shell = 0,                                         \
        .mode_params = {                                          \
            .api_mode_params = {                                  \
                .notify_callback = NULL,                          \
            }                                                     \
        }                                                         \
    }))

typedef enum
{
    FACELOCK_SAFETY_LEVEL_LOWEST = 0,      // lowesr safety level
    FACELOCK_SAFETY_LEVEL_LOW,             // low safety level
    FACELOCK_SAFETY_LEVEL_NORMAL,          // default safety level
    FACELOCK_SAFETY_LEVEL_HIGH,            // high safety level
    FACELOCK_SAFETY_LEVEL_HIGHEST          // highest safety level
} facelock_safety_level_t;

typedef struct facelock_user_info
{
    uint16_t user_num;                     // count of user
    uint16_t id[50];                       // list of user id
} facelock_user_info_t;

typedef enum
{
    FACELOCK_FOCUS_LEFT_IR_MODE = 0,
    FACELOCK_FOCUS_RIGHT_IR_MODE,
    FACELOCK_FOCUS_RGB_MODE
} facelock_focus_mode_t;

/**
 * @brief  initilize facelock module with notify callback
 * @note   notify call back is used to report current face state
 *         do not do time consuming task in this callback, facelock
 *         will wait this function return before continue
 *
 * @todo   call notify callback in another thread??
 *
 * @return  0: OK, -1: Failed
 */
int facelock_init(facelock_init_params_t init_params);

/**
 * @brief  release facelock module, not implement now
 *
 * @return  0: OK, -1: Failed
 */
int facelock_uninit();

/**
 * @brief  init video
 * @note   you can get video from usb only after init video, this function
 *         will internal been called in #facelock_init
 *
 * @return  0: OK, -1: Failed
 */
int facelock_init_video();

/**
 * @brief  set uvc parameter
 * @note   new parameter will take effect after reboot
 *
 * @return  0: OK, -1: Failed
 */
int facelock_set_uvc_param(const facelock_uvc_cfg_t *param);

/**
 * @brief  get uvc parameter
 *
 * @return  0: OK, -1: Failed
 */
int facelock_get_uvc_param(facelock_uvc_cfg_t *param);

/**
 * @brief  uninit video
 * @return  0: OK, -1: Failed
 */
int facelock_uninit_video();

/**
 * @brief  check whether video has already inited
 * @note   init video is asynchronous run in another thread, so we don't know
 *         when it will finish initialize. if we call #facelock_uninit_video
 *         the time after #facelock_init_video, video thread will not be uninited
 *         for this scenario, we can call #facelock_is_video_inited untill it
 *         return 1, then call #facelock_uninit_video
 * @return  0: OK, -1: Failed
 */
int facelock_is_video_inited();

/**
 * @brief  enroll user with given info asynchronously
 *
 * @param[in]     enroll_info      user info and enroll control info
 * @param[in]     enroll_callback  asynchronously callback function which will be called after enroll has finished
 * @note will return current face state through notify_callback which
 *       registered when call #facelock_init
 *
 * @return  0: OK, -1: Failed
 */
int facelock_enroll(const facelock_enroll_info_t *enroll_info, enroll_callback_t enroll_callback);

/**
 * @brief  reset face state
 *
 * @note   when enroll with interactive mode(facelock_enroll_info_t::enroll_type)
 *         sdk will record current enrolled face state, so you must clear face state
 *         before start a new interactive facelock_enroll
 *
 * @return  0: OK, -1: Failed
 */
int facelock_reset_enroll_state();

/**
 * @brief  verify
 *
 * @param[in]     verify_info      verify info
 * @param[in]     verify_callback  asynchronously callback function which will be called after verify has finished
 * @note will return current face state through notify_callback which
 *       registered when call #facelock_init
 *
 * @return  0: OK, -1: Failed
 */
int facelock_verify(const facelock_verify_info_t *verify_info, verify_callback_t verify_callback);

/**
 * @brief  clear and reset all in-processing face task, enter standby mode
 * 
 * @return  0: OK, -1: Failed
 */
int facelock_reset();

/**
 * @brief  delete user with given user id
 *
 * @param[in]     user_id
 * @note          will return 0 if no given user_id in face database
 *
 * @return  0: OK, -1: Failed
 */
int facelock_delete_user(uint16_t user_id);

/**
 * @brief  delete all user in face database
 * 
 * @note          will return 0 if face database is empty
 *
 * @return  0: OK, -1: Failed
 */
int facelock_delete_all_user();

/**
 * @brief  get current user number in facedatabase
 *
 * @param[in,out] user_num current user number in database
 *
 * @return  0: OK, -1: Failed
 */
int facelock_get_user_num(uint16_t *user_num);

/**
 * @brief  get current user info in facedatabase
 *
 * @param[in,out] user_info current user info in database
 *
 * @return  0: OK, -1: Failed
 */
int facelock_get_user_info(facelock_user_info_t *user_info);

/**
 * @brief  enter demo mode, not do face recognize
 *
 * @return  0: OK, -1: Failed
 */
int facelock_enter_demo_mode();

/**
 * @brief  exit demo mode
 *
 * @return  0: OK, -1: Failed
 */
int facelock_exit_demo_mode();

/** 
 * @brief  register uvc stream callback to notify uvc stream state
 * 
 * @param[in] cb  callback function pointer
 *
 * @return  0: OK, -1: Failed
 */
int facelock_register_uvc_callback(facelock_uvc_strm_cb_t cb);

/**
 * @brief  set safety level of face recognize algorithm
 *
 * @return  0: OK, -1: Failed
 */
int facelock_set_verify_safety_level(facelock_safety_level_t safety_level);

/**
 * @brief  set safety level of face liveness check algorithm
 *
 * @return  0: OK, -1: Failed
 */
int facelock_set_liveness_safety_level(facelock_safety_level_t safety_level);

/**
 * @brief  get version info of facelock
 *
 * @param[in,out] lib_version  facelock lib version
 * @param[in,out] algo_version facelock algorithm version
 * @param[in,out] cfg_version  facelock configuration file version
 * 
 * @note call #facelock_init before call this API
 * 
 * @return  0: OK, -1: Failed
 */
int facelock_get_version_info(facelock_version_t *lib_version, facelock_version_t *algo_version, facelock_version_t *cfg_version);

/**
 * @brief  enter focus mode
 *
 * @param[in] mode  focus mode
 * @param[in] sensor_dev_name  sensor factory device name 
 *
 * @note for example: sensor_dev_name: T1_sp2509_factory
 *
 * @return  0: OK, -1: Failed
 */
int facelock_enter_focus_mode(facelock_focus_mode_t mode, const char* sensor_dev_name);

#ifdef __cplusplus
}
#endif

#endif // __FACELOCK_SDK_H__

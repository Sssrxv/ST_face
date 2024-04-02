#ifndef _CIS_DEV_DRIVER_H_
#define _CIS_DEV_DRIVER_H_

#include <stdint.h>
#include "i2c.h"
#include "mipi_types.h"
#include "mipi.h"
#include "dvpin_ctl.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum cis_format {
    CIS_FORMAT_RGB_BAYER8,
    CIS_FORMAT_RGB_BAYER10,
    CIS_FORMAT_RGB_BAYER12,
    CIS_FORMAT_RGB_BAYER14,
    CIS_FORMAT_RGB_YUYV,
    CIS_FORMAT_MAX
} cis_format_t;

typedef enum cis_mclk_id {
    CIS_MCLK_ID_MCLK0,
    CIS_MCLK_ID_MCLK1,
    CIS_MCLK_ID_PWM0,
    CIS_MCLK_ID_PWM1,
    CIS_MCLK_ID_PWM2,
    CIS_MCLK_ID_PWM3,
    CIS_MCLK_ID_PWM4,
    CIS_MCLK_ID_PWM5,
    CIS_MCLK_ID_PWM6,
    CIS_MCLK_ID_PWM7,
    CIS_MCLK_ID_PWM8,
    CIS_MCLK_ID_PWM9,
    CIS_MCLK_ID_PWM10,
    CIS_MCLK_ID_PWM11
} cis_mclk_id_t;

typedef enum cis_bayer_pat {
    CIS_BAYER_PAT_RGGB = 0x0,
    CIS_BAYER_PAT_GRBG = 0x1,
    CIS_BAYER_PAT_GBRG = 0x2,
    CIS_BAYER_PAT_BGGR = 0x3,
    CIS_BAYER_PAT_MAX,
} cis_bayer_pat_t;

typedef struct cis_frame_param {
    int             width;
    int             height;
    int             framerate;
    cis_format_t    format;
    cis_bayer_pat_t bayer_pat;
} cis_frame_param_t;


typedef struct cis_exposure_param {
    union {
        float min_again;            /** minimum analog gain allowed.     (for example: 1.0 means 1x) */
        float min_again_hdr[3];     /** minimum analog gain allowed.     (for example: 1.0 means 1x) */
    };
    union {
        float max_again;            /** maximum analog gain allowed.     (for example: 8.0 means 8x) */
        float max_again_hdr[3];     /** maximum analog gain allowed.     (for example: 8.0 means 8x) */
    };
    union {
        float step_again;           /** minimum analog gain step allowd. (for example: 1.0 means 1x) */
        float step_again_hdr[3];    /** minimum analog gain step allowd. (for example: 1.0 means 1x) */
    };

    union {
        float min_dgain;            /** minimum digital gain allowed     (for example: 1.0 means 1x) */
        float min_dgain_hdr[3];     /** minimum digital gain allowed     (for example: 1.0 means 1x) */
    };
    union {
        float max_dgain;            /** maximum digital gain allowed.    (for example: 8.0 means 8x) */
        float max_dgain_hdr[3];     /** maximum digital gain allowed.    (for example: 8.0 means 8x) */
    };
    union {
        float step_dgain;           /** minimum digital gain step allowd.(for example: 1.0 means 1x) */
        float step_dgain_hdr[3];    /** minimum digital gain step allowd.(for example: 1.0 means 1x) */
    };

    union {
        float min_itime;            /** minimum integration time in seconds allowd. (for example: 0.01 means 10ms) */
        float min_itime_hdr[3];     /** minimum integration time in seconds allowd. (for example: 0.01 means 10ms) */
    };
    union {
        float max_itime;            /** maximum integration time in seconds allowd. (for example: 0.03 means 30ms) */
        float max_itime_hdr[3];     /** maximum integration time in seconds allowd. (for example: 0.03 means 30ms) */
    };
    union {
        float step_itime;           /** minimux integration time step allowd.       (for example: 0.01 means 10ms) */
        float step_itime_hdr[3];    /** minimux integration time step allowd.       (for example: 0.01 means 10ms) */
    };

    union {
        float initial_again;        /** the initial analog gain has been set */
        float initial_again_hdr[3]; /** the initial analog gain has been set */
    };
    union {
        float initial_dgain;        /** the initial digital gain has been set */
        float initial_dgain_hdr[3]; /** the initial digital gain has been set */
    };
    union {
        float initial_itime;        /** the initial itime has been set */
        float initial_itime_hdr[3]; /** the initial itime has been set */
    };
} cis_exposure_param_t;


typedef struct cis_vcm_param {
    int   has_vcm_motor;
    float min_vcm_pos;
    float max_vcm_pos;
    float min_vcm_step;
} cis_vcm_param_t;


typedef struct cis_exposure {
    union {
        float again;
        float again_hdr[3];
    };

    union {
        float dgain;
        float dgain_hdr[3];
    };

    union {
        float itime;
        float itime_hdr[3];
    };
} cis_exposure_t;


typedef struct cis_config {
    int reserved;
} cis_config_t;


typedef struct cis_mipi_param {
    int                     freq;                           // mipi lane bitrate in Mbit/s
    int                     lane_num;                       // mipi data lane number
    int                     vc_num;                         // mipi virtual channel number
    mipi_virtual_channel_t  virtual_channels[MAX_VC_NUM];   // virtual channels
    mipi_data_type_t        data_type;                      // mipi data type
} cis_mipi_param_t;


typedef struct cis_dvpi_param {
    dvpin_data_type_t format;
    uint32_t h_start;
    uint32_t v_start;
    uint32_t h_stop;
    uint32_t v_stop;
    uint32_t h_size;
    uint32_t v_size;
    uint32_t vs_pol;
    uint32_t x_stride;
    uint32_t y_stride;
} __attribute__((packed, aligned(4))) cis_dvpi_param_t;


typedef enum cis_interface_type {
    CIS_INTERFACE_TYPE_MIPI,
    CIS_INTERFACE_TYPE_DVPI
} cis_interface_type_t;


typedef struct cis_interface_param {
    cis_interface_type_t    interface_type;
    union {
        cis_mipi_param_t        mipi_param;
        cis_dvpi_param_t        dvpi_param;
    };
} cis_interface_param_t;


/** cis_dev_driver_t */
typedef struct cis_dev_driver {
    const char              *name;          /** sensor dirver name */
    i2c_device_number_t     i2c_num;        /** i2c bus this sensor connected */
	uint32_t 			    i2c_tar_addr;   /** i2c target address */
    int                     power_pin;      /** power on/off gpio pin this sensor connected */
    int                     reset_pin;      /** reset gpio pin this sensor connected */
    cis_mclk_id_t           mclk_id;        /** mclk ID */
    uint32_t                mclk_freq;      /** mclk frequency in Hz */
    int                     fps;            /** sensor fps */
    int                     mf_mode;        /** sensor mirror and flip mode */
    void                    *context;       /** pointer to  driver private context */

    int  (*init)(struct cis_dev_driver *dev_driver);
    int  (*start_stream)(struct cis_dev_driver *dev_driver, const cis_config_t *config);
    int  (*stop_stream)(struct cis_dev_driver *dev_driver);
    int  (*wake)(struct cis_dev_driver *dev_driver);
    int  (*sleep)(struct cis_dev_driver *dev_driver);    
    void (*power_on)(struct cis_dev_driver *dev_driver);
    void (*power_off)(struct cis_dev_driver *dev_driver);
    void (*reset)(struct cis_dev_driver *dev_driver);

    int  (*get_interface_param)(struct cis_dev_driver *dev_driver, cis_interface_param_t *param);
    int  (*get_exposure_param)(struct cis_dev_driver *dev_driver, cis_exposure_param_t *exp_param);
    int  (*get_vcm_param)(struct cis_dev_driver *dev_driver, cis_vcm_param_t *vcm_param);
    int  (*set_exposure)(struct cis_dev_driver *dev_driver, const cis_exposure_t *exp);

    int  (*get_frame_parameter)(struct cis_dev_driver *dev_driver, cis_frame_param_t *param);
} cis_dev_driver_t;

/** 
 * @brief  Find cis device driver by device name string
 * 
 * @param sens_dev_name        sensor device name string, for example: "ov02b_dev0", "ov02b_dev1"
 * 
 * @return sensor device driver pointer 
 */
cis_dev_driver_t  *cis_find_dev_driver(const char *sens_dev_name);

/** 
 * @brief  Find cis device driver array list by sensor type string
 * 
 * @param sens_type            sensor type string, for example: "ov02b", "sc132gs"
 * @param dev_list             device list
 * @param dev_cnt              device count to fill into device list
* 
 * @return 0 - OK, other - Fail  
 */
int cis_find_dev_driver_list(const char *sens_type, cis_dev_driver_t **dev_list, int dev_cnt);

/** 
 * @brief  Get all registered cis device driver
 * 
 * @param dev_list             device list
 * @param dev_cnt              max device count to fill into device list
 * 
 * @return 0 - OK, other - Fail  
 */
int cis_get_all_dev_driver_list(cis_dev_driver_t **dev_list, int dev_cnt);

/** 
 * @brief  cis_print_devices 
 * 
 * @param is_mipi
 */
void cis_print_devices(bool is_mipi);

/** 
 * @brief override i2c_num and i2c_tar_addr in cis dev driver
 * 
 * @param dev
 * @param i2c_num
 * @param i2c_tar_addr
 * 
 * @return  0
 */
int cis_dev_drv_config_i2c(cis_dev_driver_t *dev, int i2c_num, uint32_t i2c_tar_addr);

/** 
 * @brief override power pin and reset pin in cis dev driver
 * 
 * @param dev
 * @param power_pin
 * @param reset_pin
 * 
 * @return  0
 */
int cis_dev_drv_config_power_reset_pin(cis_dev_driver_t *dev, int power_pin, int reset_pin);

/** 
 * @brief override mclk_id and mclk_freq in cis dev driver
 * 
 * @param dev
 * @param mclk_id
 * @param mclk_freq
 * 
 * @return  0
 */
int cis_dev_drv_config_mclk(cis_dev_driver_t *dev, int mclk_id, int mclk_freq);

/** 
 * @brief override fps in cis dev driver
 * 
 * @param dev
 * @param fps
 * 
 * @return  0
 */
int cis_dev_drv_config_fps(cis_dev_driver_t *dev, int fps);

/** 
 * @brief override mirror and flip mode in cis dev driver
 * 
 * @param dev
 * @param mf_mode mirror and flip mode
 * 
 * @return  0
 */
int cis_dev_drv_config_mf_mode(cis_dev_driver_t *dev, int mf_mode);

#ifdef __cplusplus
}
#endif


#endif

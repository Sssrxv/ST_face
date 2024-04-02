#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UCLASS_GPIO = 0,                          /** Bank of general-purpose I/O pins */
    UCLASS_I2C,                               /** I2C bus */
    UCLASS_I2C_EEPROM,                        /** I2C EEPROM device */
    UCLASS_CIS_MIPI,                          /** CIS MIPI device */
    UCLASS_CIS_DVPI,                          /** CIS DVPI device */
    UCLASS_DISPLAY_DVPO,                      /** Display DVPO LCD device */
    UCLASS_DISPLAY_I80,                       /** Display I80 LCD device */
    UCLASS_DISPLAY_SPI,                       /** Display SPI LCD device */
    UCLASS_SPI,                               /** SPI bus */
    UCLASS_SPI_FLASH,                         /** SPI flash */
    UCLASS_SPI_GENERIC,                       /** SPI generic device */
    UCLASS_USB,                               /** USB bus */
    UCLASS_USB_HUB,                           /** USB hub */
    UCLASS_USB_GARGET,                        /** USB device */
    UCLASS_LED,                               /** LED device */
    UCLASS_AUDIO_CODEC,                       /** audio codec */
    UCLASS_INDEV,                             /** indev device */

    UCLASS_COUNT,
    UCLASS_INVALID = -1,
} uclass_id_t;

typedef void device_driver_t;

#ifndef SECTION
    #if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
        #define SECTION(x)                  __attribute__((used, section(x)))
    #elif defined(__ICCARM__) || defined(__ICCRX__)
        #define SECTION(x)                  @ x
    #elif defined(__GNUC__)
        #define SECTION(x)                  __attribute__((section(x)))
    #else
        #define SECTION(x)
    #endif
#endif


#define UDEVICE_EXPORT(_id, _name, _flag, _driver, _priv) \
    const char udev_##_name[] = #_name;                   \
    const struct udevice                                  \
    udevice_##_name SECTION(".udevice_list") =            \
    {                                                     \
        .name = udev_##_name,                             \
        .id   = _id,                                      \
        .flag = _flag,                                    \
        .driver = _driver,                                \
        .priv = _priv                                     \
    }


/** udevice struct */
struct udevice {
    const char            *name;              /** device name */
    uclass_id_t            id;                /** class id */
    uint32_t               flag;              /** device flag */
    device_driver_t       *driver;            /** the driver used by this device */
    void                  *priv;              /** private pointer of device */
};

/** 
 * @brief  Get device name from udevice struct
 * 
 * @param  dev[in]        pointer to device struct
 * 
 * @return device name string pointer 
 */
const char *device_get_name(const struct udevice *dev);

/** 
 * @brief  Get device uclass id from udevice struct
 * 
 * @param  dev[in]        pointer to device struct
 * 
 * @return udevice class id @ref uclass_id_t
 */
uclass_id_t device_get_uclass_id(const struct udevice *dev);

/** 
 * @brief  Get device flag from udevice struct
 * 
 * @param  dev[in]       pointer to device struct
 * 
 * @return device flag  
 */
uint32_t    device_get_flag(const struct udevice *dev);

/** 
 * @brief  Get device driver pointer
 * 
 * @param  dev[in]       pointer to device struct
 * 
 * @return device driver pointer
 */
device_driver_t *device_get_driver(const struct udevice *dev);

/** 
 * @brief Get device private pointer from udevice struct
 * 
 * @param  dev[in]       pointer to device struct
 * 
 * @return device private pointer 
 */
void *device_get_priv_pointer(const struct udevice *dev);

/** 
 * @brief Find device with uclass_id and device name
 * 
 * @param id[in]        device uclass id
 * @param name[in]      device name string
 * @param dev_pp[out]   pointer of device pointer
 * 
 * @return 0 - OK, -1 - Fail  
 */
int device_find_by_id_name(uclass_id_t id, const char *name, struct udevice **dev_pp);

/** 
 * @brief Print out device list with the specific uclass id
 * 
 * @param id[in]        device uclass id, if invalid, print out all
 * 
 * @return 0  
 */
int device_list_show(uclass_id_t id);

/** 
 * @brief  device_get_list_head_and_count 
 * 
 * @param dev_pp[out]   output device list header pointer
 * @param count[out]    device count
 * 
 * @return   
 */
int device_get_list_head_and_count(struct udevice **dev_pp, int *count);

#ifdef __cplusplus
}
#endif

#endif // __DEVICE_H__

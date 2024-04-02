#ifndef __VIDEOIN_DEV_H__
#define __VIDEOIN_DEV_H__

#include "cis_dev_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

int videoin_get_sensor_num(void);
const char* videoin_get_sensor_info_by_index(int index, int *width, int *height);
cis_dev_driver_t **videoin_get_driver_by_index(int index);
int videoin_get_sensor_index(const char* name, int width, int height);

#ifdef __cplusplus
}
#endif

#endif // __VIDEOIN_DEV_H__

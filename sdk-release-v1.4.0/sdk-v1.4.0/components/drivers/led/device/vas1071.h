#ifndef __VAS1071_H__
#define __VAS1071_H__

#include <stdbool.h>
#include <stdint.h>
#include "led_driver_abstract.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

enum vas_i_main_level {
    VAS_I_MAIN_LVL_0  = 0,      // 0    mA
    VAS_I_MAIN_LVL_1  = 1,      // 46.4 mA
    VAS_I_MAIN_LVL_2  = 2,      // 92.8 mA
    VAS_I_MAIN_LVL_3  = 3,
    VAS_I_MAIN_LVL_4  = 4,
    VAS_I_MAIN_LVL_5  = 5,
    VAS_I_MAIN_LVL_6  = 6,      // 278.4  mA
    VAS_I_MAIN_LVL_7  = 7,
    VAS_I_MAIN_LVL_8  = 8,      // 371.2  mA
    VAS_I_MAIN_LVL_9  = 9,
    VAS_I_MAIN_LVL_10 = 10,     // 464.0  mA
    VAS_I_MAIN_LVL_11 = 11,
    VAS_I_MAIN_LVL_12 = 12,
    VAS_I_MAIN_LVL_13 = 13,
    VAS_I_MAIN_LVL_14 = 14,     // 649.6  mA
    VAS_I_MAIN_LVL_15 = 15,
    VAS_I_MAIN_LVL_16 = 16,
    VAS_I_MAIN_LVL_17 = 17,     // 788.8  mA
    VAS_I_MAIN_LVL_18 = 18,     // 835.2  mA
    VAS_I_MAIN_LVL_19 = 19,     // 881.6  mA
    VAS_I_MAIN_LVL_20 = 20,     // 928.0  mA
    VAS_I_MAIN_LVL_21 = 21,     // 974.4  mA
    VAS_I_MAIN_LVL_22 = 22,     // 1020.8 mA
    VAS_I_MAIN_LVL_23 = 23,
    VAS_I_MAIN_LVL_24 = 24,
    VAS_I_MAIN_LVL_25 = 25,
    VAS_I_MAIN_LVL_26 = 26,
    VAS_I_MAIN_LVL_27 = 27,
    VAS_I_MAIN_LVL_28 = 28,
    VAS_I_MAIN_LVL_29 = 29,
    VAS_I_MAIN_LVL_30 = 30,
    VAS_I_MAIN_LVL_31 = 31,     // 1438.4 mA
    VAS_I_MAIN_LVL_MAX,
};

enum vas_i_aux_level {
    VAS_I_AUX_LVL_0 = 0,        // 1  mA
    VAS_I_AUX_LVL_1 = 1,        // 31 mA
    VAS_I_AUX_LVL_2 = 2,        // 61 mA
    VAS_I_AUX_LVL_3 = 3,
    VAS_I_AUX_LVL_4 = 4,        // 121 mA
    VAS_I_AUX_LVL_5 = 5,
    VAS_I_AUX_LVL_6 = 6,
    VAS_I_AUX_LVL_7 = 7,        // 211 mA
    VAS_I_AUX_LVL_MAX,
};

enum vas_time_out_val {
    VAS_TIMEOUT_2MS   = 0,
    VAS_TIMEOUT_4MS   = 1,
    VAS_TIMEOUT_8MS   = 2,
    VAS_TIMEOUT_16MS  = 3,
    VAS_TIMEOUT_32MS  = 4,
    VAS_TIMEOUT_64MS  = 5,
    VAS_TIMEOUT_128MS = 6,
};

#define VAS_FLASH_TIMEOUT_EN        (1UL << 2)
#define VAS_TIMEOUT_VAL(x)          ((x) << 3)

int  vas1071_init(i2c_device_number_t i2c_num, uint8_t enable_pin, bool gpio_control);

void vas1071_get_current_range(float *min_ma, float *max_ma);
void vas1071_get_current_range_wrapper(float *min, float *max, float *step);
void vas1071_get_current_step(float *current_step);
int  vas1071_set_current(float current_ma); // will handle both main and aux current
int  vas1071_set_current_wrapper(led_num_t led_num, float current_ma);

int  vas1071_set_main_current(int main_level);
int  vas1071_set_aux_current(int aux_level);
int  vas1071_set_flash_timeout(int timeout_level);
int  vas1071_set_flash_timeout_ms(int timeout_ms);

void vas1071_power_on(void);
void vas1071_power_off(void);

int  vas1071_flash_bright(void *param);

void vas1071_suspend(void);
void vas1071_resume(void);

#ifdef __cplusplus
}
#endif

#endif // __VAS1071_H__

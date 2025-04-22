#include "vas1071.h"
#include "gpio.h"
#include "aiva_sleep.h"
#include "syslog.h"
#include <math.h>

#define VAS1071_ADDR                    (0x4D)

//
#define CURRENT_MAIN_STEP_MA            (46.4f)
#define CURRENT_AUX_STEP_MA             (30.0f)
#define CURRENT_MAIN_MAX_MA             (CURRENT_MAIN_STEP_MA * (VAS_I_MAIN_LVL_MAX-1))
#define CURRENT_AUX_MAX_MA              (211.0f)
//
#define CURRENT_MIN_MA                  (0.0f)
#define CURRENT_MAX_MA                  (CURRENT_MAIN_MAX_MA + CURRENT_AUX_MAX_MA)

static i2c_device_number_t m_i2c_num = I2C_DEVICE_MAX;
static uint8_t m_enable_pin          = GPIO_MAX_PINNO;
static int m_suspend_flag            = 0;
static bool m_gpio_control = true;

static int vas1071_write_reg(i2c_device_number_t dev, uint8_t reg, uint8_t data)
{
    int ret;
    uint8_t data_buf[2];
    
    data_buf[0] = reg;
    data_buf[1] = data;
    ret = i2c_send_data(dev, VAS1071_ADDR, data_buf, 2);

    return ret;
}

int vas1071_init(i2c_device_number_t i2c_num, uint8_t enable_pin, bool gpio_control)
{
    int ret;

    /*i2c_init(i2c_num, VAS1071_ADDR, 7, 50*1000);*/
    i2c_init(i2c_num, VAS1071_ADDR, 7, 800*1000);

    if (gpio_control) {
        gpio_set_drive_mode(enable_pin, GPIO_DM_OUTPUT);
    } else {
        gpio_set_drive_mode(enable_pin, GPIO_DM_INPUT);
    }

    m_i2c_num    = i2c_num;
    m_enable_pin = enable_pin;
    m_gpio_control = gpio_control;

    // config slow mode
    ret = vas1071_write_reg(m_i2c_num, 0x2, 0x2);

    return ret;
}

// void vas1071_get_current_range(int *min, int *max)
// {
//     *min = VAS_I_MAIN_LVL_0;
//     *max = VAS_I_MAIN_LVL_MAX + VAS_I_AUX_LVL_MAX - 1;
// }

void vas1071_get_current_range(float *min_ma, float *max_ma)
{
    *min_ma = CURRENT_MIN_MA;
    *max_ma = CURRENT_MAX_MA;
}


void vas1071_get_current_range_wrapper(float *min, float *max, float *step)
{
    vas1071_get_current_range(min, max);
    vas1071_get_current_step(step);
}


void vas1071_get_current_step(float *current_step)
{
    // NOTE: only return CURRENT_MAIN_STEP_MA
    *current_step = CURRENT_MAIN_STEP_MA;
}

int vas1071_set_current(float current_ma)
{
    int ret = -1;
    int main_level = 0;
    int aux_level = 0;

    //
    if (current_ma < 0 || current_ma > CURRENT_MAX_MA)
    {
        LOGE(__func__, "invalid current:%f\n", current_ma);
        return ret;
    }

    if (current_ma <= CURRENT_MAIN_MAX_MA)
    {
        main_level = (int)(roundf(current_ma / CURRENT_MAIN_STEP_MA));
        ret = vas1071_set_main_current(main_level);
    }
    else
    {
        ret = vas1071_set_main_current(VAS_I_MAIN_LVL_MAX-1);
        if (ret == 0)
        {
            aux_level = (int)(roundf((current_ma - CURRENT_MAIN_MAX_MA) / CURRENT_AUX_STEP_MA));
            ret = vas1071_set_aux_current(aux_level);
        }
    }

    LOGD(__func__, "current %f mA, main level:%d, aux level:%d\n", current_ma, main_level, aux_level);

    return ret;
}


int vas1071_set_current_wrapper(led_num_t led_num, float current_ma)
{
    return vas1071_set_current(current_ma);
}


int vas1071_set_main_current(int main_level)
{
    int ret;
    vas1071_init(m_i2c_num, m_enable_pin, m_gpio_control);
    ret = vas1071_write_reg(m_i2c_num, 0x0, main_level);
    return ret;
}

int vas1071_set_aux_current(int aux_level)
{
    int ret;
    vas1071_init(m_i2c_num, m_enable_pin, m_gpio_control);
    ret = vas1071_write_reg(m_i2c_num, 0x1, aux_level);
    return ret;
}

int vas1071_set_flash_timeout(int timeout_level)
{
    int ret;
    uint8_t val = VAS_TIMEOUT_VAL(timeout_level) | VAS_FLASH_TIMEOUT_EN;

    ret = vas1071_write_reg(m_i2c_num, 0x2, val);
    return 0;
}

static enum vas_time_out_val timeout_ms2level(int timeout_ms)
{
    enum vas_time_out_val timeout_level = VAS_TIMEOUT_2MS;
    if (timeout_ms >= 128)
        timeout_level = VAS_TIMEOUT_128MS;
    else if (timeout_ms >=64)
        timeout_level = VAS_TIMEOUT_64MS;
    else if (timeout_ms >=32)
        timeout_level = VAS_TIMEOUT_32MS;
    else if (timeout_ms >=16)
        timeout_level = VAS_TIMEOUT_16MS;
    else if (timeout_ms >=8)
        timeout_level = VAS_TIMEOUT_8MS;
    else if (timeout_ms >=4)
        timeout_level = VAS_TIMEOUT_4MS;
    else
        timeout_level = VAS_TIMEOUT_2MS;

    return timeout_level;
}

int vas1071_set_flash_timeout_ms(int timeout_ms)
{
    enum vas_time_out_val timeout_level = timeout_ms2level(timeout_ms);
    return vas1071_set_flash_timeout(timeout_level);
}

void vas1071_power_on(void)
{
    if (m_enable_pin != GPIO_MAX_PINNO) {
        gpio_set_pin(m_enable_pin, GPIO_PV_HIGH);
    }
}

void vas1071_power_off(void)
{
    if (m_enable_pin != GPIO_MAX_PINNO) {
        gpio_set_pin(m_enable_pin, GPIO_PV_LOW);
    }
}

int vas1071_flash_bright(void *param)
{
    if (!m_suspend_flag) {
        vas1071_power_off();
        vas1071_power_on();
    }
    return 0;
}

void vas1071_suspend(void)
{
    m_suspend_flag = 1;
    vas1071_power_off();
}

void vas1071_resume(void)
{
    m_suspend_flag = 0;
    vas1071_power_on();
}

void vas1071_self_test(void)
{
    vas1071_init(I2C_DEVICE_2, GPIO_PIN29, m_gpio_control);
    vas1071_set_main_current(VAS_I_MAIN_LVL_20);
    vas1071_set_flash_timeout(VAS_TIMEOUT_32MS);
    while (1) {
        vas1071_power_off();
        aiva_msleep(100);
        vas1071_power_on();
        aiva_msleep(100);
    }
}

#include "i2c.h"
#include "syslog.h"
#include "aiva_sleep.h"
#include "gpio.h"
#include "indev_ft6336u.h"
#include "udevice.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/semaphore.h"
#endif

#include "indev.h"

#if USE_RTOS
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

// static int ft6336u_write_reg(int i2c_device, uint8_t reg_addr, uint8_t reg_val)
// {
//     int ret;
//     uint8_t data_buf[2];
//     data_buf[0] = reg_addr & 0xff;
//     data_buf[1] = reg_val & 0xff;
//     ret = i2c_send_data(i2c_device, FT6336U_I2C_ADDR, data_buf, 2);
//     if (ret < 0) {
//         LOGE(__func__, "send data error: ret is %d\n", ret);
//     }
//     return ret;
// }

static int ft6336u_read_reg(int i2c_device, uint8_t reg_addr, uint8_t *reg_val)
{
    int ret = 0;

    uint8_t addr_buf[1];
    addr_buf[0] = reg_addr & 0xff;
    // ret = i2c_send_data(i2c_device, FT6336U_I2C_ADDR, addr_buf, 1);
    ret = i2c_send_data_dma(i2c_device, FT6336U_I2C_ADDR, addr_buf, 1);
    if (ret < 0) {
        LOGE(__func__, "send error %d", ret);
    }
    // ret = i2c_recv_data(i2c_device, FT6336U_I2C_ADDR, NULL, 0, reg_val, 1);
    ret = i2c_recv_data_dma(i2c_device, FT6336U_I2C_ADDR, NULL, 0, reg_val, 1);
    if (ret < 0) {
        LOGE(__func__, "recv error");
    }

    return ret;
}

static int ft6336u_get_lock(void)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_lock(&g_mutex);
    }
#endif
    return ret;
}

static int ft6336u_release_lock(void)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_unlock(&g_mutex);
    }
#endif
    return ret;
}

int ft6336u_tp_irq_cb(void *ctx)
{
    int ret = 0;
    LOGI(__func__, "ft6336u tp cb entry.......");

    return ret;
}

static int tf6336u_register_tp_irq(indev_dev_t* indev)
{
    int ret = 0;
    gpio_init();
    gpio_set_drive_mode(indev->irq_pin_num, GPIO_DM_INPUT);
    gpio_irq_register(indev->irq_pin_num, GPIO_INTR_POS_EDGE, ft6336u_tp_irq_cb, NULL);

    return ret;
}

static int tf6336u_device_test(int i2c_device)
{
    int ret = 0;
    uint8_t reg_value = 0;
    ret = ft6336u_read_reg(i2c_device, FT6336U_ADDR_CHIP_ID_H, &reg_value);
    // LOGI(__func__, "read dev id:0x%x.", reg_value);
    if(reg_value != 0x64) {
        LOGE(__func__, "read dev id wrong 0x%x, should be(0x64).", reg_value);
        return ret;
    }
    else {
        LOGI(__func__, "dev id 0x%x", reg_value);
    }

    // ret = ft6336u_read_reg(i2c_device, FT6336U_ADDR_DEVICE_MODE, &reg_value);
    // LOGD(__func__, "read device mode:0x%x.", reg_value);

    return 0;
}

static int indev_tp_tf6336u_init(indev_dev_t* indev)
{
    int ret = 0;
    i2c_device_number_t i2c_num = indev->i2c_num;

    if(indev->is_initialized) {
        LOGE(__func__, "dev is already inited!");
        return -1;
    }

    ft6336u_get_lock();

    indev->ops->reset(indev);

    i2c_init(i2c_num, FT6336U_I2C_ADDR, 7, 400*1000);

    ret = tf6336u_device_test(i2c_num);
    if(ret != 0) {
        LOGE(__func__, "get ID failed!\n");
        return ret;
    }

    ret = tf6336u_register_tp_irq(indev);

    indev->is_initialized = 1;
    ft6336u_release_lock();

    return ret;
}

static int indev_tp_ft6336u_reset(indev_dev_t* indev)
{
    int ret = 0;
    gpio_set_drive_mode(indev->rst_pin_num, GPIO_DM_OUTPUT);
    gpio_set_pin(indev->rst_pin_num, GPIO_PV_HIGH);
    aiva_msleep(200);
    gpio_set_pin(indev->rst_pin_num, GPIO_PV_LOW);
    aiva_msleep(20);
    gpio_set_pin(indev->rst_pin_num, GPIO_PV_HIGH);
    aiva_msleep(500);

    return ret;
}

static int indev_tp_tf6336u_release(indev_dev_t* indev)
{
    int ret = 0;
    ft6336u_get_lock();
    indev->is_enable = 0;
    indev->is_initialized = 0;
    ft6336u_release_lock();
    return ret;
}

int indev_tp_tf6336u_enable(indev_dev_t* indev)
{
    int ret = 0;

    ft6336u_get_lock();
    indev->is_enable = 1;
    ft6336u_release_lock();

    return ret;
}

int indev_tp_tf6336u_disable(indev_dev_t* indev)
{
    int ret = 0;

    ft6336u_get_lock();
    indev->is_enable = 0;
    ft6336u_release_lock();

    return ret;
}

static int indev_tp_tf6336u_get_pressed_pos(indev_dev_t* indev, int* x, int* y)
{
    int ret = 0;
    uint8_t reg_value = 0;
    uint16_t pos_x = 0, pos_y = 0;
    uint8_t pos_xh = 0, pos_yh = 0;
    uint8_t pos_xl = 0, pos_yl = 0;

    if(indev->is_initialized == 0 || indev->is_enable == 0) {
        LOGE(__func__, "initialized, enable not OK");
        return -1;
    }

    ret = ft6336u_read_reg(indev->i2c_num, FT6336U_ADDR_TD_STATUS, &reg_value);
    if(reg_value == 0) 
    {
        // Not any pressed.
        return -2;
    }
    // LOGI(__func__, "read TD status:0x%x.", reg_value);

    ret |= ft6336u_read_reg(indev->i2c_num, FT6336_ADDR_TOUCH1_XL, &pos_xl);
    ret |= ft6336u_read_reg(indev->i2c_num, FT6336_ADDR_TOUCH1_XH, &pos_xh);
    ret |= ft6336u_read_reg(indev->i2c_num, FT6336_ADDR_TOUCH1_YL, &pos_yl);
    ret |= ft6336u_read_reg(indev->i2c_num, FT6336_ADDR_TOUCH1_YH, &pos_yh);
    pos_x = ((pos_xh & 0x0F)<<8)|pos_xl;
    pos_y = ((pos_yh & 0x0F)<<8)|pos_yl;
    *x = (int)pos_x;
    *y = (int)pos_y;

    // LOGI(__func__, "read pos(0x%x, 0x%x).", pos_x, pos_y);
    return ret;
}


static indev_ops_t indev_tp_ft6336u_ops = {
    .init = indev_tp_tf6336u_init,
    .reset = indev_tp_ft6336u_reset,
    .release = indev_tp_tf6336u_release,
    .enable = indev_tp_tf6336u_enable,
    .disable = indev_tp_tf6336u_disable,
    .get_pressed_pos = indev_tp_tf6336u_get_pressed_pos,
    .set_pressed_pos = NULL,        // only for sim
};

static indev_dev_t indev_tp_tf6336u = {
    .name = "indev_tp_tf6336u",
    .ops = &indev_tp_ft6336u_ops,
    .is_initialized = 0,
    .is_enable = 0,
    .type = INDEV_TYPE_TOUCHPAD,
    .i2c_num = I2C_DEVICE_3,
    .i2c_addr_wr = FT6336U_I2C_ADDR,
    .irq_pin_num = GPIO_PIN10,
    .rst_pin_num = GPIO_PIN19,
    .priv = NULL,
};

UDEVICE_EXPORT(UCLASS_INDEV,     indev_tp_tf6336u,    0,      &indev_tp_tf6336u,    NULL);



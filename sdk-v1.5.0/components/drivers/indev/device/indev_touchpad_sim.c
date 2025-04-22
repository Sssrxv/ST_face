#include "udevice.h"
#include <stdint.h>
#include <stdio.h>
#include "indev.h"
#include "indev_touchpad_sim.h"


static int pressed_pos_width = 0;
static int pressed_pos_height = 0;

static touchpad_priv_t tpad_data = {
    .is_pressed = 0,
    .x = 0,
    .y = 0,
    .pad_max_w = 480,
    .pad_max_h = 864,
};

int indev_touchpad_init(indev_dev_t *dev)
{
    dev->is_initialized = 1;
    return 0;
}

int indev_touchpad_release(indev_dev_t *dev)
{
    dev->is_initialized = 0;
    dev->is_enable = 0;
    return 0;
}

int indev_touchpad_enable(indev_dev_t *dev)
{
    dev->is_enable = 1;
    return 0;
}

int indev_touchpad_disable(indev_dev_t *dev)
{
    dev->is_enable = 0;
    return 0;
}

int indev_touchpad_get_pressed_pos(indev_dev_t *dev, int *x, int *y)
{
    if(dev->is_initialized == 0 || dev->is_enable == 0) {
        LOGE(__func__, "preesed, initialized, enable not OK");
        return -1;
    }

    *x = tpad_data.x;
    *y = tpad_data.y;
    tpad_data.is_pressed = 0;

    return 0;
}

int sim_touchpad_set_pressed_pos(indev_dev_t *dev, int x, int y)
{
    // LOGI(__func__, "indev driver set pos(%d, %d).", w, h);
    if(dev->is_initialized == 0 || dev->is_enable == 0) {
        LOGE(__func__, "preesed, initialized, enable not OK");
        return -1;
    }
    if(tpad_data.pad_max_w < x || tpad_data.pad_max_h < y) {
        LOGE(__func__, "set pos data FAILED.");
        return -1;
    }

    tpad_data.x = x;
    tpad_data.y = y;
    tpad_data.is_pressed = 1;

    return 0;
}

static indev_ops_t indev_touchpad_ops = {
    .init = indev_touchpad_init,
    .release = indev_touchpad_release,
    .enable = indev_touchpad_enable,
    .disable = indev_touchpad_disable,
    .get_pressed_pos = indev_touchpad_get_pressed_pos,
    .set_pressed_pos = sim_touchpad_set_pressed_pos,
};

static indev_dev_t touchpad_sim = {
    .name = "touchpad_sim",
    .ops = &indev_touchpad_ops,
    .is_initialized = 0,
    .is_enable = 0,
    .type = INDEV_TYPE_TOUCHPAD,
    .priv = &tpad_data,
};

UDEVICE_EXPORT(UCLASS_INDEV,     touchpad_sim,    0,      &touchpad_sim,    NULL);

INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/led
INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/led/device

ifeq ($(CONFIG_GPIO_LED_DRIVER),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/gpio_led.c
# SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/gpio_led_driver.c
endif

ifeq ($(CONFIG_LM3644),y)
# SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/lm3644.c
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/lm3644_2.c
endif

ifeq ($(CONFIG_VAS1071),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/vas1071.c
endif

ifeq ($(CONFIG_PHX3D8014),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/led/device/phx3d8014.c
endif

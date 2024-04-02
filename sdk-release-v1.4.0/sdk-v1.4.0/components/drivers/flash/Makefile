INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/flash
INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/flash/nand_flash/device
INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/flash/nor_flash/device


ifeq ($(CONFIG_GD5FXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nand_flash/device/gd5fxx.c
endif

ifeq ($(CONFIG_MX35LFXXE4),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nand_flash/device/mx35lfxxe4.c
endif

ifeq ($(CONFIG_W25NXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nand_flash/device/w25nxx.c
endif

ifeq ($(CONFIG_GD25QXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/gd25qxx.c
endif

ifeq ($(CONFIG_MX25LXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/mx25lxx.c
endif

ifeq ($(CONFIG_W25QXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/w25qxx.c
endif

ifeq ($(CONFIG_P25QXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/p25qxx.c
endif

ifeq ($(CONFIG_XT25QXX),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/xt25qxx.c
endif
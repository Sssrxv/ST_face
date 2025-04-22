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

ifeq ($(CONFIG_NOR_FLASH_STD),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/flash/nor_flash/device/nor_flash_std.c
endif

PROJ_ROOT   := .
SDK_ROOT    :=$(PROJ_ROOT)/../../sdk
TOOLS_ROOT  :=$(SDK_ROOT)/tools
PROJ_NAME   := display_example

include $(TOOLS_ROOT)/common_setting.mk

all: elf bin img

INC_APP_DIR  += -I$(PROJ_ROOT)


ifeq ($(CONFIG_USE_DISPLAY_DVPOUT),y)
SOURCE_APP_C += $(PROJ_ROOT)/display_dvpout_example.c
endif

ifeq ($(CONFIG_USE_DISPLAY_I80),y)
SOURCE_APP_C += $(PROJ_ROOT)/display_i80_example.c
endif

ifeq ($(CONFIG_USE_DISPLAY_LVGL),y)
SOURCE_APP_C += $(PROJ_ROOT)/display_lvgl_example.c
SOURCE_APP_C += $(PROJ_ROOT)/lvgl_case/font_test.c
SOURCE_APP_C += $(PROJ_ROOT)/lvgl_case/btnmatrix_test.c
include $(PROJ_ROOT)/lvgl_case/benchmark/Makefile
endif

include $(TOOLS_ROOT)/build.mk

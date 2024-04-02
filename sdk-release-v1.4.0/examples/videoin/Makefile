PROJ_ROOT   := .
SDK_ROOT    :=$(PROJ_ROOT)/../../sdk
TOOLS_ROOT  :=$(SDK_ROOT)/tools
PROJ_NAME   := videoin_example

include $(TOOLS_ROOT)/common_setting.mk

all: elf bin img

INC_APP_DIR  += -I$(PROJ_ROOT)
INC_APP_DIR  += -I$(PROJ_ROOT)/inc

ifeq ($(CONFIG_USE_MIPI_MONO_UVC),y)
SOURCE_APP_C += $(PROJ_ROOT)/os_mono_uvc_app.c
endif

ifeq ($(CONFIG_USE_MIPI_STEREO_UVC),y)
SOURCE_APP_C += $(PROJ_ROOT)/os_stereo_uvc_app.c
endif

ifeq ($(CONFIG_USE_DVPI_MONO_UVC),y)
SOURCE_APP_C += $(PROJ_ROOT)/os_dvpi_mono_uvc_app.c
endif

ifeq ($(CONFIG_USE_DVPI_MONO_MIPI_STEREO_UVC),y)
SOURCE_APP_C += $(PROJ_ROOT)/os_dvpi_mono_mipi_stereo_uvc_app.c
endif

ifeq ($(CONFIG_USE_DVPI_MONO_MIPI_MONO_MIPI_STEREO_UVC),y)
SOURCE_APP_C += $(PROJ_ROOT)/os_dvpi_mono_mipi_mono_mipi_stereo_uvc_app.c
endif

SOURCE_APP_C += $(PROJ_ROOT)/os_mipi_cam_comm.c

include $(TOOLS_ROOT)/build.mk

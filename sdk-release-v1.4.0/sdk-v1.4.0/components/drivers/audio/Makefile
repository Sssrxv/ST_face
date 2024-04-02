INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/audio
INC_LIB_DIR  += -I$(SDK_ROOT)/components/drivers/audio/device

ifeq ($(CONFIG_AUDIO_ES8374),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/audio/device/audio_es8374.c
endif

ifeq ($(CONFIG_AUDIO_ES8311),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/audio/device/audio_es8311.c
endif

ifeq ($(CONFIG_AUDIO_ES8388),y)
SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/audio/device/audio_es8388.c
endif

SOURCE_LIB_C += $(SDK_ROOT)/components/drivers/audio/audio_dev.c


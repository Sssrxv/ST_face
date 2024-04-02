PROJ_ROOT   := .
SDK_ROOT    :=$(PROJ_ROOT)/../../sdk
TOOLS_ROOT  :=$(SDK_ROOT)/tools
PROJ_NAME   := uart_example

include $(TOOLS_ROOT)/common_setting.mk

all: elf bin img

INC_APP_DIR  += -I$(PROJ_ROOT)
SOURCE_APP_C += $(PROJ_ROOT)/uart_example.c
SOURCE_APP_C += $(PROJ_ROOT)/uart_stream_buffer.c

include $(TOOLS_ROOT)/build.mk

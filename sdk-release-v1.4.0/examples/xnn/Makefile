PROJ_ROOT   := .
SDK_ROOT    :=$(PROJ_ROOT)/../../sdk
TOOLS_ROOT  :=$(SDK_ROOT)/tools
PROJ_NAME   := xnn_inference_example

include $(TOOLS_ROOT)/common_setting.mk

all: elf bin img

INC_APP_DIR  += -I$(PROJ_ROOT)
SOURCE_APP_CPP += $(PROJ_ROOT)/xnn_inference.cpp

include $(TOOLS_ROOT)/build.mk

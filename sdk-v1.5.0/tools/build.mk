
# FW image definitions (Decimal)
# xx.xx.xxxx - current: 0.0.1
# make sure major minor and patch version num is not longer than 2 digtals
# to avoid exceed the maximum version string buffer
TARGET_FW_VER    ?= 0x00000001
# year month and day
TARGET_FW_ADDR   ?= 0x80000000
TARGET_FW_BAUDR  ?= 2
TARGET_FW_DMAEN  ?= 1
TARGET_FW_PLL_BP ?= 0
TARGET_FW_EXC_EN ?= 0
TARGET_FW_RX_DLY ?= 0
DEPEND ?= dependencies_file
SEGGER_RTT_GDB ?= SEGGER_RTT.gdb

APP_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE_APP_C:.c=.o)))
APP_OBJS += $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE_APP_CPP:.cpp=.o)))
LIB_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE_LIB_C:.c=.o)))
LIB_OBJS += $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE_LIB_S:.S=.o)))
LIB_OBJS += $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE_LIB_CPP:.cpp=.o)))


$(BIN_DIR)/$(TARGET_LIB): $(OBJ_DIR) $(BIN_DIR) $(LIB_OBJS) $(LIBSDK) $(DEPEND)
	$(AR) r $(BIN_DIR)/$(TARGET_LIB) $(LIB_OBJS)


$(BIN_DIR)/$(TARGET_ELF):  $(APP_OBJS) $(BIN_DIR)/$(TARGET_LIB) $(LIBSDK) $(DEPEND)
	@echo Linking $@
	@echo "$(APP_OBJS)"
	@echo "$(BIN_DIR)/$(TARGET_LIB)"
	# make sure udevice in lib link to final elf
	$(eval DEPEND_LIBS_UDEVICE_LINK_FLAGS = $(shell $(NM) $(DEPEND_LIBS) $(LIBSDK) $(BIN_DIR)/$(TARGET_LIB) | grep udevice_ | cut -d ' ' -f 3 | sed -e 's/^/-Wl,-u,/' | tr '\n' ' '))
	$(eval DEPEND_LIBS_SHELLCMD_LINK_FLAGS = $(shell $(NM) $(DEPEND_LIBS) $(LIBSDK) $(BIN_DIR)/$(TARGET_LIB) | grep shellCmd | cut -d ' ' -f 3 | sed -e 's/^/-Wl,-u,/' | tr '\n' ' '))
	$(CC) $(CFLAGS) $(APP_OBJS) -Wl,--start-group $(BIN_DIR)/$(TARGET_LIB) $(LIBSDK) $(DEPEND_LIBS_UDEVICE_LINK_FLAGS) $(DEPEND_LIBS_SHELLCMD_LINK_FLAGS) $(DEPEND_LIBS) $(LFLAGS) -Wl,--end-group -Wl,-M > $@_list -o $@

$(BIN_DIR)/$(TARGET_BIN): $(BIN_DIR)/$(TARGET_ELF)
	@$(MAKENOISE) Generating $@
	$(OBJCOPY) --gap-fill 0 -O binary $(OBJCOPY_SRAM_FLAGS) $< $@

$(BIN_DIR)/$(TARGET_IMG): $(BIN_DIR)/$(TARGET_BIN)
	$(BIN2IMAGE) -v $(TARGET_FW_VER) -i $(BIN_DIR)/$(TARGET_BIN) -s $(TARGET_FW_ADDR) -o $@ -f 0 -b $(TARGET_FW_BAUDR) -d $(TARGET_FW_DMAEN) -e $(TARGET_FW_EXC_EN) -p $(TARGET_FW_PLL_BP) -r $(TARGET_FW_RX_DLY)
	@gzip -c -9 $(BIN_DIR)/$(TARGET_IMG) > $(BIN_DIR)/$(TARGET_IMG).gz
	#cat $(BIN_DIR)/$(TARGET_ELF)_list | grep _segger_rtt_start | cut -d ' ' -f 17
	$(eval RTT_ADDR = $(shell cat $(BIN_DIR)/$(TARGET_ELF)_list | grep _segger_rtt_start | cut -d ' ' -f 17))
	@echo "define set_rtt_addr\n    mon exec SetRTTAddr $(RTT_ADDR)\nend\n" > $(BIN_DIR)/$(SEGGER_RTT_GDB)

$(BIN_DIR)/$(TARGET_DIS): $(BIN_DIR)/$(TARGET_ELF)
	@$(MAKENOISE) Generating $(BIN_DIR)/$(TARGET_DIS)
	$(OBJDUMP) -D $<  > $@

$(BIN_DIR)/$(TARGET_LIST): $(BIN_DIR)/$(TARGET_ELF)
	@$(MAKENOISE) Generating $(BIN_DIR)/$(TARGET_LIST)
	$(OBJDUMP) -S $<  > $@

.PHONY: $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR):
	@$(MAKENOISE) "Creating object directory "$(OBJ_DIR)
	@mkdir -p $@

$(BIN_DIR):
	@$(MAKENOISE) "Creating binary directory "$(BIN_DIR)
	@mkdir -p $@

# https://www.gnu.org/software/make/manual/html_node/Double_002dColon.html
clean::
	echo "Removing object files "$(OBJ_DIR)
	rm -rf $(OBJ_DIR)
	echo "Cleaning content of "$(BIN_DIR)
	rm -rf $(BIN_DIR)
	echo "Removing dependency file " $(PROJ_ROOT)/$(DEPEND)
	@-rm -f $(PROJ_ROOT)/$(DEPEND)
	echo "Removing part file "$(PROJ_ROOT)/part.bin
	rm -f $(PROJ_ROOT)/part.bin

menuconfig:
	$(eval PROJ_ABS_ROOT = $(shell realpath $(PROJ_ROOT)))
	$(eval SDK_ABS_ROOT = $(shell realpath $(SDK_ROOT)))
	# $(SDK_ABS_ROOT)/tools/menuconfig/mconf $(SDK_ABS_ROOT)/components/Kconfig
	env MENUCONFIG_STYLE=aquatic $(TOOLS_ROOT)/menuconfig/menuconfig.py $(PROJ_ABS_ROOT)/Kconfig
	$(TOOLS_ROOT)/post_chip_select.sh $(PROJ_ABS_ROOT)/.config $(SDK_ABS_ROOT)

ifeq (1,$(SDK_INCLUDE_WITH_SRC_CODE))
ifeq (,$(filter clean menuconfig,$(MAKECMDGOALS)))
ifndef MAKE_RESTARTS
# $(warning "not restart")

FORCE:
$(SDK_ROOT)/scripts/Makefile.libflags : FORCE
	$(eval PROJ_ABS_ROOT = $(shell realpath $(PROJ_ROOT)))
	@$(MAKE) -C $(SDK_SRC_ROOT) PROJ_ROOT=${PROJ_ABS_ROOT}

$(SDK_ROOT)/scripts/Makefile.libincs : $(SDK_ROOT)/scripts/Makefile.libflags
# .NOTPARALLEL: $(SDK_ROOT)/scripts/Makefile.libincs $(SDK_ROOT)/scripts/Makefile.libflags

.ONESHELL:
$(DEPEND): $(SDK_ROOT)/scripts/Makefile.libincs $(SDK_ROOT)/scripts/Makefile.libflags
	@echo 'Checking C source dependencies'
	echo "sdk is included as source code, get libflags and libincs from new sdk file"
	$(eval INC_SDK_DIR = $(shell cat $(SDK_ROOT)/scripts/Makefile.libincs | cut -d ' ' -f 3 | tr '\n' ' '))
	$(eval SDK_CFLAGS = $(shell cat $(SDK_ROOT)/scripts/Makefile.libflags | cut -d ' ' -f 3 | tr '\n' ' '))
	$(CC) -MM $(CFLAGS) $(SDK_CFLAGS) $(INC_APP_DIR) $(INC_LIB_DIR) $(INC_SDK_DIR) $(SOURCE_APP_C) $(SOURCE_APP_CPP) $(SOURCE_LIB_C) $(SOURCE_LIB_CPP) \
		| sed -E 's@(.*)\.o@$(OBJ_DIR)/\1.o@g' > $@.new
	if [ -f $@ ] && [ -z $(shell cmp -s "$@.new" "$@") ]; then
		rm $@.new
	else
		if [ -f $@ ]; then rm $@; fi
		mv $@.new $@
	fi
else # ifndef MAKE_RESTARTS
# $(warning "restart")
.ONESHELL:
$(DEPEND): $(SOURCE_APP_C) $(SOURCE_APP_CPP) $(SOURCE_LIB_C) $(SOURCE_LIB_CPP)
	# @echo "sdk included as lib"
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $(INC_APP_DIR) $(INC_LIB_DIR) $(INC_SDK_DIR) $^ \
		| sed '\''s@\(.*\)\.o[ :]@$(OBJ_DIR)/\1.o$(SUFFIX):@g'\'' > $@'
endif # ifndef MAKE_RESTARTS
endif # ifeq (,$(filter clean menuconfig,$(MAKECMDGOALS)))
else # ifeq (1,$(SDK_INCLUDE_WITH_SRC_CODE))
.ONESHELL:
$(DEPEND): $(SOURCE_APP_C) $(SOURCE_APP_CPP) $(SOURCE_LIB_C) $(SOURCE_LIB_CPP)
	# @echo "sdk included as lib"
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $(INC_APP_DIR) $(INC_LIB_DIR) $(INC_SDK_DIR) $^ \
		| sed '\''s@\(.*\)\.o[ :]@$(OBJ_DIR)/\1.o$(SUFFIX):@g'\'' > $@'
endif # ifeq (1,$(SDK_INCLUDE_WITH_SRC_CODE))

.PHONY: all elf img lib dis list
elf:  $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/$(TARGET_ELF)
img:  $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/$(TARGET_IMG)
lib:  $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/$(TARGET_LIB)
dis:  $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/$(TARGET_DIS)
list: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/$(TARGET_LIST)


################################################
# Rules
################################################

define cmd-compile-app-c
$(OBJ_DIR)/$(notdir $(1:.c=.o)): $(MODULES) $(1) Makefile
	@echo Building $(1)
	$(CC) $(CFLAGS) -D__FILENAME__=\"$(notdir $(1))\" $(WERROR) $(INC_APP_DIR) $(INC_SDK_DIR) $(INC_LIB_DIR) -c -o $(OBJ_DIR)/$(notdir $(1:.c=.o)) $(1)
endef

define cmd-compile-app-cpp
$(OBJ_DIR)/$(notdir $(1:.cpp=.o)): $(MODULES) $(1) Makefile
	@echo Building $(1)
	$(CPPC) $(CFLAGS) -D__FILENAME__=\"$(notdir $(1))\" $(CXXFLAGS) $(INC_APP_DIR) $(INC_SDK_DIR) $(INC_LIB_DIR) -c -o $(OBJ_DIR)/$(notdir $(1:.cpp=.o)) $(1)
endef

define cmd-compile-lib-s
$(OBJ_DIR)/$(notdir $(1:.S=.o)): $(MODULES) $(1) Makefile
	@$(MAKENOISE) Building $(1)
	$(CC) $(CFLAGS) -D__FILENAME__=\"$(notdir $(1))\" $(INC_LIB_DIR) -c -O2 -o $(OBJ_DIR)/$(notdir $(1:.S=.o)) $(1)
endef

define cmd-compile-lib-c
$(OBJ_DIR)/$(notdir $(1:.c=.o)): $(MODULES) $(1) Makefile
	@echo Building $(1)
	$(CC) $(CFLAGS) -D__FILENAME__=\"$(notdir $(1))\" $(WERROR) $(INC_SDK_DIR) $(INC_LIB_DIR) -c -o $(OBJ_DIR)/$(notdir $(1:.c=.o)) $(1)
endef

define cmd-compile-lib-cpp
$(OBJ_DIR)/$(notdir $(1:.cpp=.o)): $(MODULES) $(1) Makefile
	@echo Building $(1)
	$(CPPC) $(CFLAGS) -D__FILENAME__=\"$(notdir $(1))\" $(CXXFLAGS) $(INC_SDK_DIR) $(INC_LIB_DIR) -c -o $(OBJ_DIR)/$(notdir $(1:.cpp=.o)) $(1)
endef

# Build for %.S files
$(foreach file, $(SOURCE_LIB_S), $(eval $(call cmd-compile-lib-s, $(file))))

# Build for %.c files
$(foreach file, $(SOURCE_APP_C), $(eval $(call cmd-compile-app-c, $(file))))

# Build for %.cpp files
$(foreach file, $(SOURCE_APP_CPP), $(eval $(call cmd-compile-app-cpp, $(file))))

# Build for %.c files
$(foreach file, $(SOURCE_LIB_C), $(eval $(call cmd-compile-lib-c, $(file))))

# Build for %.cpp files
$(foreach file, $(SOURCE_LIB_CPP), $(eval $(call cmd-compile-lib-cpp, $(file))))

# Include dependencies
ifeq (,$(filter clean menuconfig,$(MAKECMDGOALS)))
-include $(DEPEND)
endif

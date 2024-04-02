PROJ_ROOT   := .
SDK_ROOT    :=$(PROJ_ROOT)/../../sdk
TOOLS_ROOT  :=$(SDK_ROOT)/tools
PROJ_NAME   := rtos_example

include $(TOOLS_ROOT)/common_setting.mk

TARGET_ROM_NOR  :=$(PROJ_NAME)_nor.rom
BIN2ROM:=$(TOOLS_ROOT)/bin2rom/bin2rom
SWAPDW:=$(TOOLS_ROOT)/swapdw/swapdw
MKLFS:=$(TOOLS_ROOT)/mklfs/mklfs

all: elf bin img rom

INC_APP_DIR  += -I$(PROJ_ROOT)
SOURCE_APP_C += $(PROJ_ROOT)/rtos_example.c

# libfacelock
INC_APP_DIR  += -I$(PROJ_ROOT)/../../lib/libfacelock
INC_APP_DIR  += -I$(PROJ_ROOT)/../../lib

# libfacelock
DEPEND_LIBS  += $(PROJ_ROOT)/../../lib/libfacelock/libfacelock.a

SOURCE_APP_C += $(PROJ_ROOT)/../../lib/libfacelock/facelock_sdk_test.c

.PHONY: rom
rom: $(BIN_DIR)/$(TARGET_ROM_NOR)

$(BIN_DIR)/$(TARGET_ROM_NOR): $(BIN_DIR)/$(TARGET_IMG)
	cp cfg/*.cfg $(BIN_DIR)
	cp $(TOOLS_ROOT)/bin2rom/spl/*.img $(BIN_DIR)
	@gzip -c -9 $(BIN_DIR)/$(TARGET_IMG) > $(BIN_DIR)/$(TARGET_IMG).gz
	$(SWAPDW) $(BIN_DIR)/$(TARGET_IMG) $(BIN_DIR)/fw0
	$(SWAPDW) $(BIN_DIR)/$(TARGET_IMG) $(BIN_DIR)/fw1
	# 524288 is filesystem size, must match the value in .cfg file
	$(MKLFS) -c fs_pack -b 4096 -r 256 -p 256 -s 524288 -i $(BIN_DIR)/fs.bin
	$(SWAPDW) $(BIN_DIR)/fs.bin $(BIN_DIR)/fs
	$(BIN2ROM) $(BIN_DIR)/nor_spl.img $(BIN_DIR)/part_nor.cfg 0 $(BIN_DIR)/$(TARGET_ROM_NOR)
	@gzip -c -9 $(BIN_DIR)/$(TARGET_ROM_NOR) > $(BIN_DIR)/$(TARGET_ROM_NOR).gz

include $(TOOLS_ROOT)/build.mk

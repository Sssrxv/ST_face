ALL_SUBDIRS := $(wildcard */)

# BUILD_SUBDIRS = $(filter-out videoin/,$(ALL_SUBDIRS))
BUILD_SUBDIRS = $(ALL_SUBDIRS)

.PHONY : all clean $(BUILD_SUBDIRS)
all : $(BUILD_SUBDIRS)

.NOTPARALLEL:
$(BUILD_SUBDIRS) :
	@$(MAKE) -C $@ || exit 1

.ONESHELL:
clean:
	@for DIR in $(BUILD_SUBDIRS); do
		$(MAKE) -C $$DIR clean || exit 1;
	done

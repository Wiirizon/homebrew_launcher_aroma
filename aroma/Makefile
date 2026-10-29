# Native Aroma build. The root Makefile still builds the historical ELF.
.DEFAULT_GOAL := all
ifeq ($(strip $(DEVKITPRO)),)
$(error Set DEVKITPRO to your devkitPro installation)
endif
RPXLOADER ?= $(DEVKITPRO)/wut/usr
TARGET := homebrew_launcher_aroma
BUILD := build
APP_NAME := Simple Aroma Launcher (experimental)
APP_SHORTNAME := Simple Aroma Launcher
APP_AUTHOR := Community contributors
NODE ?= node
include $(DEVKITPRO)/wut/share/wut_rules
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Werror -ffunction-sections -fdata-sections $(MACHDEP) \
            -I$(RPXLOADER)/include -I$(WUT_ROOT)/include
LDFLAGS := $(MACHDEP) $(RPXSPECS) -Wl,--gc-sections,-Map,$(BUILD)/$(TARGET).map
LIBPATHS := -L$(RPXLOADER)/lib -L$(WUT_ROOT)/lib
LIBS := -lrpxloader -lwut
LD := $(CXX)
OFILES := $(BUILD)/main.o $(BUILD)/catalog.o

.PHONY: all clean
all: $(TARGET).wuhb

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: source/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(TARGET).elf: $(OFILES) $(RPXLOADER)/lib/librpxloader.a
$(TARGET).rpx: $(TARGET).elf scripts/sanitize-elf.mjs
	$(NODE) scripts/sanitize-elf.mjs $<
	elf2rpl $< $@
$(TARGET).wuhb: $(TARGET).rpx Makefile

clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.d $(BUILD)/*.map $(TARGET).elf $(TARGET).rpx $(TARGET).wuhb $(TARGET).lst

-include $(OFILES:.o=.d)

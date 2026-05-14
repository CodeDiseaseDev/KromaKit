MACOS_APP ?= Synthem
MACOS_OUT ?= build_macos/$(MACOS_APP)
MACOS_BUILD_DIR ?= build_macos/obj

MACOS_TOOLCHAIN ?= /home/code/procursus/cctools-port/usage_examples/macos_toolchain/target
MACOS_TOOLCHAIN_BIN := $(MACOS_TOOLCHAIN)/bin
MACOS_SDK ?= /home/code/SDKs/MacOSX14.5.sdk

MACOS_CC := $(MACOS_TOOLCHAIN_BIN)/x86_64-apple-darwin-clang
MACOS_CXX := $(MACOS_TOOLCHAIN_BIN)/x86_64-apple-darwin-clang++

MACOS_COMMON_FLAGS = \
	-target x86_64-apple-darwin \
	-B$(MACOS_TOOLCHAIN_BIN) \
	-ferror-limit=100 \
	-isysroot $(MACOS_SDK) \
	-mmacosx-version-min=14.0 \
	-w \
	-MMD \
	-MP

MACOS_CXXFLAGS = $(COMMON_CPP_STD) $(COMMON_OPT_CFLAGS) $(MACOS_COMMON_FLAGS)
MACOS_OBJCXXFLAGS = $(MACOS_CXXFLAGS) -fobjc-arc

MACOS_FRAMEWORKS = \
	-framework AppKit \
	-framework Foundation \
	-framework QuartzCore \
	-framework Metal \
	-framework MetalPerformanceShaders \
	-framework AVFoundation \
	-framework AudioToolbox


#	-fuse-ld=lld
#	$(if $(filter release,$(BUILD)),-flto,)
MACOS_LDFLAGS = \
	-target x86_64-apple-darwin \
	-B$(MACOS_TOOLCHAIN_BIN) \
	-isysroot $(MACOS_SDK) \
	-mmacosx-version-min=14.0 \
	-Wl,-no_adhoc_codesign \
	-Wl,-platform_version,macos,14.0,14.5 \


MACOS_OBJ = $(MACOS_SRC:%=$(MACOS_BUILD_DIR)/%.o)

macos: $(MACOS_OUT)

$(MACOS_OUT): $(MACOS_OBJ)
	@mkdir -p $(dir $@)
	$(MACOS_CXX) $(MACOS_OBJ) -o $@ $(MACOS_FRAMEWORKS) $(MACOS_LDFLAGS)

$(MACOS_BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(MACOS_CXX) $(MACOS_CXXFLAGS) -c $< -o $@

$(MACOS_BUILD_DIR)/%.mm.o: %.mm
	@mkdir -p $(dir $@)
	$(MACOS_CXX) $(MACOS_OBJCXXFLAGS) -c $< -o $@

run-macos: macos
	@echo "Built macOS binary at $(MACOS_OUT). Running on Linux host is not supported."

bundle-macos: macos
	@echo "Not implemented yet. Build raw Mach-O first, then add bundling."

clean-macos:
	rm -rf build_macos

-include $(MACOS_OBJ:.o=.d)

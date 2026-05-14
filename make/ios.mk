# make/ios.mk

APP = Synthem
OUT = build_package/$(APP).rawexec

TOOLCHAIN = /home/code/procursus/cctools-port/usage_examples/ios_toolchain/target/bin
CLANG = $(TOOLCHAIN)/aarch64-apple-darwin-clang++

SDK = $(HOME)/SDKs/iPhoneOS13.2.sdk

BUILD_DIR = build

IOS_COMMON_CFLAGS = -std=c++23 \
                    -ferror-limit=100 \
                    -isysroot $(SDK) \
                    -fobjc-arc \
                    -w \
                    -MMD -MP

IOS_CXXFLAGS = $(IOS_COMMON_CFLAGS) $(OPT_CFLAGS)
IOS_CFLAGS = $(IOS_CXXFLAGS)

FRAMEWORKS = -framework UIKit \
             -framework Foundation \
             -framework AVFoundation \
             -framework AudioToolbox \
             -framework Metal \
             -framework MetalPerformanceShaders \
             -framework MetalKit \
             -framework QuartzCore \
             -framework CoreGraphics

IOS_LDFLAGS = -fuse-ld=lld \
              -isysroot $(SDK) \
              -Wl,-platform_version,ios,13.0,16.0 \
              $(if $(filter release,$(BUILD)),-flto,)

IOS_OBJ = $(IOS_SRC:%=$(BUILD_DIR)/%.o)

IOS_AUDIO_SRC = $(filter synthem/%,$(IOS_SRC))
IOS_AUDIO_OBJ = $(IOS_AUDIO_SRC:%=$(BUILD_DIR)/%.o)

$(IOS_AUDIO_OBJ): IOS_CXXFLAGS += $(if $(filter release,$(BUILD)),-ffast-math,)

ios: $(OUT)

$(OUT): $(IOS_OBJ)
	@mkdir -p $(dir $@)
	$(CLANG) $(IOS_OBJ) -o $@ $(FRAMEWORKS) $(IOS_LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CLANG) $(IOS_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.mm.o: %.mm
	@mkdir -p $(dir $@)
	$(CLANG) $(IOS_CXXFLAGS) -c $< -o $@

-include $(IOS_OBJ:.o=.d)

debug:
	$(MAKE) ios BUILD=debug -j$(shell nproc)

release:
	$(MAKE) ios BUILD=release -j$(shell nproc)

clean-ios:
	rm -rf $(BUILD_DIR) $(OUT)

test:
	$(MAKE) -C tests test
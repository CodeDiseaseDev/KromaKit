# make/windows.mk
#
# Windows / MinGW build target.
#
# This currently reuses the existing GLFW/OpenGL desktop backend files
# that live under directui/platform/linux.
#
# TODO:
# - Rename platform/linux OpenGL files to platform/desktop if they stay shared.
# - Add a real main_windows.cpp if Windows needs different startup/resource paths.
# - Add a Windows-specific font/resource loader if FontLoaderLinux becomes too Linuxy.

WINDOWS_APP = Synthem
WINDOWS_OUT = build_windows/$(WINDOWS_APP).exe
WINDOWS_BUILD_DIR = build_windows/obj

WINDOWS_CXX ?= x86_64-w64-mingw32-g++

#	-O0 \
#	-g3 \
#-DDEBUG
WINDOWS_CXXFLAGS = \
	-std=c++23 \
	-fno-omit-frame-pointer \
	-fno-inline \
	-DWIN32_LEAN_AND_MEAN \
	-DNOMINMAX \
	-DGL_GLEXT_PROTOTYPES=1 \
	-DM_PI=3.14159265358979323846 \
	-Wall \
	-Wextra \
	-MMD \
	-MP \
	-I. \
	-Ithird_party/glad/include



# If your MinGW GLFW package is installed somewhere custom, override these:
#
# make windows WINDOWS_EXTRA_INCLUDES="-I/path/to/glfw/include" \
#              WINDOWS_EXTRA_LIBDIRS="-L/path/to/glfw/lib"
WINDOWS_EXTRA_INCLUDES ?=
WINDOWS_EXTRA_LIBDIRS ?=

WINDOWS_CXXFLAGS += $(WINDOWS_EXTRA_INCLUDES)

# Dynamic/shared GLFW builds usually need glfw3.dll beside the .exe at runtime.
# Static GLFW builds may need the larger dependency list below.
WINDOWS_LDFLAGS = \
	$(WINDOWS_EXTRA_LIBDIRS) \
	-lglfw3 \
	-lopengl32 \
	-lgdi32 \
	-luser32 \
	-lkernel32 \
	-lshell32 \
	-lwinmm \
	-limm32 \
	-lole32 \
	-luuid \
	-lversion \
	-lsetupapi \
    -lavrt \
    -Wl,-subsystem,windows

#compile as static
WINDOWS_LDFLAGS += -static-libgcc -static-libstdc++ -static

# For first bring-up, keep a Windows-specific host/backend path.
# NanoVG GL3 loader wiring is still pending for MinGW.
WINDOWS_PLATFORM_SRC = \
	main_windows.cpp \
	directui/platform/windows/NanoVGGL3Backend.cpp \
	directui/platform/windows/OpenGLNanoVGContextOwner.cpp \
	directui/platform/windows/OpenGLRenderBackendHooks.cpp \
	directui/platform/linux/FontLoaderLinux.cpp \
	synthem/HapticsNoop.cpp \
	third_party/glad/src/gl.c \
    synthem/audio/platforms/windows/AudioBackendFactoryWindows.cpp \
    synthem/audio/platforms/windows/WindowsAudioBackend.cpp \
    synthem/audio/platforms/windows/WasapiAudioBackend.cpp \
    directui/platform/windows/DUIClipboardWindows.cpp

WINDOWS_SRC = \
	$(WINDOWS_PLATFORM_SRC) \
	$(DESKTOP_GLFW_HOST_SRC) \
	$(NANOVG_SHARED_SRC) \
	$(DIRECTUI_SHARED_SRC) \
	$(DIRECTUI_EFFECTS_SRC) \
	$(DIRECTUI_DEMO_CONTROLS_SRC) \
	$(SYNTHEM_SHARED_SRC)

WINDOWS_OBJ = $(WINDOWS_SRC:%=$(WINDOWS_BUILD_DIR)/%.o)

windows: $(WINDOWS_OUT)

$(WINDOWS_OUT): $(WINDOWS_OBJ)
	@mkdir -p $(dir $@)
	$(WINDOWS_CXX) $(WINDOWS_OBJ) -o $@ $(WINDOWS_LDFLAGS)

$(WINDOWS_BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(WINDOWS_CXX) $(WINDOWS_CXXFLAGS) -c $< -o $@

$(WINDOWS_BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(WINDOWS_CXX) $(WINDOWS_CXXFLAGS) -c $< -o $@

-include $(WINDOWS_OBJ:.o=.d)

run-windows: windows
	wine ./$(WINDOWS_OUT)

clean-windows:
	rm -rf build_windows

LINUX_APP = Synthem
LINUX_OUT = build_linux/$(LINUX_APP)
LINUX_BUILD_DIR = build_linux/obj

LINUX_CXX = clang++
LINUX_CXXFLAGS = -std=c++23 -O0 -g3 -fno-omit-frame-pointer -fno-inline -DDEBUG -Wall -Wextra -MMD -MP -I.
LINUX_LDFLAGS = -lglfw -lGL -ldl -lpthread -lm -lasound

LINUX_SRC = \
$(LINUX_PLATFORM_SRC) \
$(DESKTOP_GLFW_HOST_SRC) \
$(NANOVG_SHARED_SRC) \
$(DIRECTUI_SHARED_SRC) \
$(DIRECTUI_EFFECTS_SRC) \
$(DIRECTUI_DEMO_CONTROLS_SRC) \
$(SYNTHEM_SHARED_SRC)

LINUX_OBJ = $(LINUX_SRC:%=$(LINUX_BUILD_DIR)/%.o)

linux: $(LINUX_OUT)

$(LINUX_OUT): $(LINUX_OBJ)
	@mkdir -p $(dir $@)
	$(LINUX_CXX) $(LINUX_OBJ) -o $@ $(LINUX_LDFLAGS)

$(LINUX_BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(LINUX_CXX) $(LINUX_CXXFLAGS) -c $< -o $@

-include $(LINUX_OBJ:.o=.d)

run-linux: linux
	./$(LINUX_OUT)

clean-linux:
	rm -rf build_linux

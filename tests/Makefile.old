BUILD_DIR = ../build_tests
CORE_BIN = $(BUILD_DIR)/directui_tests
CONTROL_BIN = $(BUILD_DIR)/headless_control_tests

CXX = clang++

CXXFLAGS = -std=c++23 \
	-Wall \
	-Wextra \
	-g \
	-Wno-deprecated-declarations \
	-I..

CORE_TEST_CXXFLAGS = $(CXXFLAGS) -DTEST_SUITE_NAME=\"CoreLogic\"
CONTROL_TEST_CXXFLAGS = $(CXXFLAGS) -DTEST_SUITE_NAME=\"HeadlessControls\"

CORE_SOURCES = \
	TestMain.cpp \
	CoreLogicTests.cpp \
	../directui/styling/DUIStyleName.cpp \
	../directui/Logging.cpp \
	../directui/graphics/types/DUIInsets.cpp \
	../directui/graphics/types/DUIPoint.cpp \
	../directui/graphics/types/DUIRect.cpp \
	../directui/graphics/types/DUISize.cpp \
	../synthem/editor/model/Pattern.cpp \
	../synthem/editor/model/PlaylistItem.cpp \
	../synthem/editor/model/PlaylistTrack.cpp \
	../synthem/editor/model/PatternPlaylistItem.cpp \
	../synthem/editor/model/PlaylistArrangement.cpp

CONTROL_SOURCES = \
	TestMain.cpp \
	HeadlessControlTests.cpp \
	StyleRegistryTests.cpp \
	stubs/GraphicsHeadlessStubs.cpp \
	stubs/DUIWindowHeadlessStubs.cpp \
	stubs/BackdropBlurManagerHeadlessStub.cpp \
	stubs/RootStackCoordinatorHeadlessStub.cpp \
	../directui/Label.cpp \
	../directui/Panel.cpp \
	../directui/Control.cpp \
	../directui/ControlTree.cpp \
	../directui/ControlLayout.cpp \
	../directui/IDraggableControl.cpp \
	../directui/grid_layout_panel/GridLayoutPanel.cpp \
	../directui/layout/layout_types/IGridLayout.cpp \
	../directui/DUIFont.cpp \
	../directui/styling/DUIStyleName.cpp \
	../directui/core/properties/control_property_registry/PropertyRegistry.cpp \
	../directui/animation/FloatAnimation.cpp \
	../directui/graphics/types/DUIInsets.cpp \
	../directui/graphics/types/DUIPoint.cpp \
	../directui/graphics/types/DUIRect.cpp \
	../directui/graphics/types/DUISize.cpp \
	../synthem/editor/ISequence.cpp

.PHONY: test test-core test-label clean

test: test-label
test: test-core

test-core: $(CORE_BIN)
	./$(CORE_BIN)

test-label: $(CONTROL_BIN)
	./$(CONTROL_BIN)

$(CORE_BIN): $(CORE_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CORE_TEST_CXXFLAGS) $(CORE_SOURCES) -o $(CORE_BIN)

$(CONTROL_BIN): $(CONTROL_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CONTROL_TEST_CXXFLAGS) $(CONTROL_SOURCES) -o $(CONTROL_BIN)

clean:
	rm -rf $(BUILD_DIR)

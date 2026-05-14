# make/sources.mk
#
# Shared and platform-specific source lists.
# This file should be included before make/ios.mk and make/linux.mk.

# -------------------------------------------------------------------
# DirectUI shared sources
# -------------------------------------------------------------------

DIRECTUI_SHARED_SRC = \
directui/Control.cpp \
directui/ControlTree.cpp \
directui/ControlLayout.cpp \
directui/Button.cpp \
directui/graphics/Graphics.cpp \
directui/DUIFont.cpp \
directui/Panel.cpp \
directui/DUIWindow.cpp \
directui/ScrollBar.cpp \
directui/stack_panels/VStackPanel.cpp \
directui/stack_panels/HStackPanel.cpp \
directui/Application.cpp \
directui/Label.cpp \
directui/KeyboardHandler.cpp \
directui/Checkbox.cpp \
directui/HorizontalScrollBar.cpp \
directui/OpenFileDialog.cpp \
directui/IDraggableControl.cpp \
directui/ModalPresentation.cpp \
directui/scrollable_stack_panels/ScrollableVStackPanel.cpp \
directui/scrollable_stack_panels/ScrollableHStackPanel.cpp \
directui/scrollable_stack_panels/FreeScrollVStackPanel.cpp \
directui/gesture/scrolling/SwipeMomentum.cpp \
directui/gesture/SwipeInteraction.cpp \
directui/gesture/SwipeActionButton.cpp \
directui/animation/FloatEaseAnimation.cpp \
directui/animation/FloatAnimation.cpp \
directui/debug/FPSLabel.cpp \
directui/Separator.cpp \
directui/GroupBox.cpp \
directui/TextBlock.cpp \
directui/context_menu/IContextMenuItem.cpp \
directui/context_menu/ContextMenu.cpp \
directui/context_menu/item_types/ContextMenuTextItem.cpp \
directui/context_menu/item_types/ContextMenuSeparator.cpp \
directui/context_menu/item_types/ContextMenuSubMenuItem.cpp \
directui/presentation/ModalPresentationSurface.cpp \
directui/presentation/PresentationHost.cpp \
directui/overlay_background_layer/OverlayStack.cpp \
directui/overlay_background_layer/ContentStack.cpp \
directui/RootStackCoordinator.cpp \
directui/overlay_background_layer/OverlayBackgroundLayer.cpp \
directui/overlay_background_layer/IOverlayControl.cpp \
directui/Rendering/RenderPipeline.cpp \
directui/styling/DUIStyleName.cpp \
directui/graphics/types/DUIInsets.cpp \
directui/graphics/types/DUIPoint.cpp \
directui/graphics/types/DUIRect.cpp \
directui/graphics/types/DUISize.cpp \
directui/core/properties/control_property_registry/PropertyRegistry.cpp \
directui/layout/layout_types/IHStackLayout.cpp \
directui/layout/layout_types/IStackLayout.cpp \
directui/layout/layout_types/IVStackLayout.cpp \
directui/layout/layout_types/IGridLayout.cpp \
directui/grid_layout_panel/GridLayoutPanel.cpp \
directui/Rendering/render_state/RenderCache.cpp \
directui/gesture/scrolling/ScrollableView.cpp \
directui/gesture/scrolling/ScrollState.cpp \
directui/presentation/FloatingWindowControl.cpp \
directui/presentation/FloatingWindowSurface.cpp \
directui/graphics/svg/SvgLoader.cpp \
directui/graphics/GraphicsSvg.cpp \
directui/graphics/svg/SvgResourceCache.cpp \
directui/svg/SvgIcon.cpp \
directui/Logging.cpp \
directui/IconButton.cpp \
directui/text_input/TextBox.cpp \
directui/platform/DUIKeyboard.cpp \
directui/layout/keyboard_aware/KeyboardAwarePanel.cpp \
directui/TextInputBridge.cpp

#/home/code/Coding/test_ios_app/directui/presentation/FloatingWindowControl.cpp

DESKTOP_GLFW_HOST_SRC = \
directui/platform/desktop/DUIDesktopGLFWHost.cpp \
directui/platform/desktop/DesktopResourceLoader.cpp

# -------------------------------------------------------------------
# Shared render/effects/control demo sources
# -------------------------------------------------------------------

DIRECTUI_EFFECTS_SRC = \
Effects/BackdropBlurManager.cpp \
Effects/BackdropBlurPanel.cpp \
Backend/BlurPass.cpp

#DIRECTUI_DEMO_CONTROLS_SRC = \
#controls/TestControl.cpp \
#controls/AnimationControl.cpp

# -------------------------------------------------------------------
# Synthem shared sources
# -------------------------------------------------------------------

#SYNTHEM_SHARED_SRC = \
#synthem/Synthem.cpp \
#synthem/theme/SynthemTheme.cpp \
#synthem/theme/controls/DevToolsStyles.cpp \
#synthem/VUMeter.cpp \
#synthem/ShapeDisplay.cpp \
#synthem/RadialSlider.cpp \
#synthem/editor/ISequence.cpp \
#synthem/editor/PatternEditor.cpp \
#synthem/editor/PlaylistEditor.cpp \
#synthem/editor/PatternPlaylistItemControl.cpp \
#synthem/editor/PlaylistTrackHeaderControl.cpp \
#synthem/editor/PlaylistTrackHeaderPanel.cpp \
#synthem/editor/PlaylistTimelineRuler.cpp \
#synthem/editor/model/Pattern.cpp \
#synthem/editor/model/PlaylistItem.cpp \
#synthem/editor/model/PatternPlaylistItem.cpp \
#synthem/editor/model/PlaylistTrack.cpp \
#synthem/editor/model/PlaylistArrangement.cpp \
#synthem/playback/PlaybackTransport.cpp \
#synthem/playback/PlaylistPlaybackScheduler.cpp \
#synthem/project/SynthemProject.cpp \
#synthem/theme/controls/AboutStyle.cpp \
#synthem/ParameterStepper.cpp \
#synthem/Oscillator.cpp \
#synthem/MixerChannel.cpp \
#synthem/Mixer.cpp \
#synthem/Logging.cpp \
#synthem/IAudioGenerator.cpp \
#synthem/Effect.cpp \
#synthem/ClippingIndicator.cpp \
#synthem/AudioWaveShape.cpp \
#synthem/AudioEngine.cpp \
#synthem/AudioController.cpp \
#synthem/effects/Distortion.cpp \
#synthem/effects/LPF.cpp \
#synthem/menu/About.cpp \
#synthem/menu/OscillatorMenu.cpp \
#synthem/menu/devtools/devtools.cpp \
#synthem/menu/devtools/DevToolsTreeRow.cpp \
#synthem/menu/pattern_editor/PatternEditorMenu.cpp \
#synthem/menu/pattern_list/PatternList.cpp \
#synthem/menu/pattern_list/PatternListItem.cpp \
#synthem/menu/playlist_editor/PlaylistEditorMenu.cpp \
#synthem/editor/PatternNote.cpp \
#synthem/effects/ui_elements/FilterResponseGraph.cpp \
#synthem/onboarding/IntroAnimation.cpp \
#synthem/onboarding/Setup.cpp \
#synthem/audio/platforms/null/NullAudioBackend.cpp \
#synthem/AppRootControl.cpp

# Old / intentionally excluded for now:
# synthem/PatternMenu.cpp

# -------------------------------------------------------------------
# Platform-neutral third-party/core renderer source
# -------------------------------------------------------------------

NANOVG_SHARED_SRC = \
nanovg/nanovg.cpp

# -------------------------------------------------------------------
# iOS platform sources
# -------------------------------------------------------------------

IOS_PLATFORM_SRC = \
main.mm \
directui/platform/ios/DUIIOSMetalHost.mm \
directui/platform/ios/IOSResourceLoader.mm \
directui/platform/ios/FontLoader.mm \
directui/platform/ios/MetalNanoVGContextOwner.cpp \
directui/platform/ios/MetalRenderBackendHooks.cpp \
nanovg/nanovg_mtl.mm \
directui/platform/ios/DUIClipboardiOS.mm
#synthem/IOSAudioBackend.mm \
#synthem/Haptics.mm \
#synthem/audio/platforms/apple/AppleAudioSession.mm \
#synthem/audio/platforms/apple/CoreAudioHelpers.mm \
#synthem/audio/platforms/ios/AudioBackendFactoryIOS.mm \
#synthem/audio/platforms/ios/IOSAudioBackend.mm \


# -------------------------------------------------------------------
# Linux platform sources
# -------------------------------------------------------------------

LINUX_PLATFORM_SRC = \
main_linux.cpp \
directui/platform/linux/OpenGLNanoVGContextOwner.cpp \
directui/platform/linux/OpenGLRenderBackendHooks.cpp \
directui/platform/linux/NanoVGGL3Backend.cpp \
directui/platform/linux/FontLoaderLinux.cpp \
directui/platform/desktop/DUIClipboardGLFW.cpp
#synthem/HapticsNoop.cpp \
#synthem/audio/platforms/linux/AudioBackendFactoryLinux.cpp \
#synthem/audio/platforms/linux/LinuxAudioBackend.cpp \
#synthem/audio/platforms/linux/AlsaAudioBackend.cpp \
#synthem/audio/platforms/linux/PipeWireAudioBackend.cpp \


# -------------------------------------------------------------------
# macOS platform sources
# -------------------------------------------------------------------

MACOS_PLATFORM_SRC = \
main_macos.mm \
directui/platform/macos/DUIMacOSMetalHost.mm \
directui/platform/macos/MacOSResourceLoader.mm \
directui/platform/macos/FontLoaderMacOS.mm \
directui/platform/ios/MetalNanoVGContextOwner.cpp \
directui/platform/ios/MetalRenderBackendHooks.cpp \
directui/platform/macos/DUIClipboard_macOS.mm \
nanovg/nanovg_mtl.mm \
#synthem/IOSAudioBackend.mm \
#synthem/HapticsNoop.cpp \
#synthem/audio/platforms/apple/AppleAudioSession.mm \
#synthem/audio/platforms/apple/CoreAudioHelpers.mm \
#synthem/audio/platforms/macos/AudioBackendFactoryMacOS.mm \
#synthem/audio/platforms/macos/MacOSAudioBackend.mm \


# -------------------------------------------------------------------
# Final platform source lists
# -------------------------------------------------------------------

IOS_SRC = \
$(IOS_PLATFORM_SRC) \
$(NANOVG_SHARED_SRC) \
$(DIRECTUI_SHARED_SRC) \
$(DIRECTUI_EFFECTS_SRC)
#$(DIRECTUI_DEMO_CONTROLS_SRC)
#$(SYNTHEM_SHARED_SRC)

LINUX_SRC = \
$(LINUX_PLATFORM_SRC) \
$(NANOVG_SHARED_SRC) \
$(DIRECTUI_SHARED_SRC) \
$(DIRECTUI_EFFECTS_SRC)
#$(DIRECTUI_DEMO_CONTROLS_SRC)
#$(SYNTHEM_SHARED_SRC)

MACOS_SRC = \
$(MACOS_PLATFORM_SRC) \
$(NANOVG_SHARED_SRC) \
$(DIRECTUI_SHARED_SRC) \
$(DIRECTUI_EFFECTS_SRC)
#$(DIRECTUI_DEMO_CONTROLS_SRC)
#$(SYNTHEM_SHARED_SRC)

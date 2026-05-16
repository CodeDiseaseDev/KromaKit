//
// Created by code on 5/6/26.
//

#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <kromakit/platform/macos/DUIMacOSMetalHost.h>

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <memory>
#include <utility>

#include <nanovg.h>
#include <nanovg_mtl.h>

#include <kromakit/Utils.h>
#include <kromakit/styling/DUIDefaultStyles.h>
#include <kromakit/platform/apple/MetalNanoVGContextOwner.h>
#include <kromakit/platform/DUIKeyboard.h>

namespace {

std::unique_ptr<DUIMacOSMetalHostConfig> gConfig;

DUIWindow gWindow;
Control* gRootControl = nullptr;
MetalNanoVGContextOwner gRenderTargetOwner;

DUIKeyboard duiKeyboard_;
//MacOSKeyboard keyboard_;

bool IsNormalScaleEnabled() {
  return gConfig == nullptr || gConfig->normalScale;
}

int PreferredFramesPerSecond() {
  if (gConfig == nullptr || gConfig->preferredFramesPerSecond <= 0) {
    return 120;
  }

  return gConfig->preferredFramesPerSecond;
}

void SizeRootControlToWindow() {
  if (gRootControl == nullptr)
    return;

  gRootControl->SetLayoutLocation({0.0f, 0.0f});
  gRootControl->SetLayoutSize(gWindow.size);
}

bool InitMacOSKeyboard() {
  /*keyboard_.OnKeyDown = [](const DUIKeyEvent& event) {
    duiKeyboard_.OnKeyDown(event);
  };

  keyboard_.OnKeyUp = [](const DUIKeyEvent& event) {
    duiKeyboard_.OnKeyUp(event);
  };

  keyboard_.OnTextInput = [](const DUITextInputEvent& event) {
    duiKeyboard_.OnTextInput(event);
  };*/

  duiKeyboard_.OnKeyDownEvent = [](const DUIKeyEvent& event) {
    gWindow.OnKeyDown(event);
  };

  duiKeyboard_.OnKeyUpEvent = [](const DUIKeyEvent& event) {
    gWindow.OnKeyUp(event);
  };

  duiKeyboard_.OnTextInput = [](const DUITextInputEvent& event) {
    gWindow.OnTextInput(event);
  };

  return true;
}

void RedirectLogsIfEnabled() {
  if (gConfig == nullptr || !gConfig->redirectLogs) {
    return;
  }

  const char* logsFile = gConfig->logFilePath.empty()
    ? "/tmp/synthem.log"
    : gConfig->logFilePath.c_str();

  int logFd = open(logsFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (logFd < 0) {
    return;
  }

  dup2(logFd, STDOUT_FILENO);
  dup2(logFd, STDERR_FILENO);

  if (logFd > STDERR_FILENO) {
    close(logFd);
  }

  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);

  std::ios::sync_with_stdio(true);

  Logging::Log("Logs redirected to %s\n", logsFile);
}

void RegisterStylesIfConfigured() {
  if (gConfig != nullptr && gConfig->registerStyles != nullptr) {
    gConfig->registerStyles();
  }

  if (gWindow.GetContentStack() != nullptr) {
    gWindow.GetContentStack()->UseStyle(
      DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Panel
    );
  }

  if (gWindow.GetOverlayStack() != nullptr) {
    gWindow.GetOverlayStack()->UseStyle(
      DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear
    );
  }
}

void RegisterHapticsIfConfigured() {
  if (gConfig != nullptr && gConfig->registerHaptics != nullptr) {
    gConfig->registerHaptics();
  }
}

void ConfigureWindowIfConfigured() {
  if (gConfig != nullptr && gConfig->configureWindow != nullptr) {
    gConfig->configureWindow(gWindow);
  }
}

} // namespace

@interface MacMetalHostView : NSView
@property(strong, nonatomic) NSTrackingArea* trackingArea;
@end

@implementation MacMetalHostView

- (instancetype)initWithFrame:(NSRect)frameRect {
  self = [super initWithFrame:frameRect];

  if (self) {
    self.wantsLayer = YES;

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    self.layer = metalLayer;
  }

  return self;
}

- (BOOL)isFlipped {
  return YES;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)viewDidMoveToWindow {
  [super viewDidMoveToWindow];

  if (self.window != nil) {
    [self.window setAcceptsMouseMovedEvents:YES];
    [self.window makeFirstResponder:self];
  }
}

- (void)updateTrackingAreas {
  [super updateTrackingAreas];

  if (self.trackingArea != nil) {
    [self removeTrackingArea:self.trackingArea];
    self.trackingArea = nil;
  }

  NSTrackingAreaOptions options =
    NSTrackingMouseEnteredAndExited |
    NSTrackingMouseMoved |
    NSTrackingActiveInKeyWindow |
    NSTrackingInVisibleRect;

  self.trackingArea =
    [[NSTrackingArea alloc] initWithRect:NSZeroRect
                                 options:options
                                   owner:self
                                userInfo:nil];

  [self addTrackingArea:self.trackingArea];
}

- (CGFloat)directUIInputScale {
  CGFloat realScale = self.window != nil
    ? self.window.backingScaleFactor
    : [NSScreen mainScreen].backingScaleFactor;

  if (realScale <= 0.0) {
    realScale = 1.0;
  }

  return IsNormalScaleEnabled() ? 1.0 : realScale;
}

- (NSPoint)directUIPointFromEvent:(NSEvent*)event {
  NSPoint p = [self convertPoint:event.locationInWindow fromView:nil];
  CGFloat inputScale = [self directUIInputScale];

  return NSMakePoint(
    p.x * inputScale,
    p.y * inputScale
  );
}

- (void)sendMouseMoveFromEvent:(NSEvent*)event {
  NSPoint p = [self directUIPointFromEvent:event];

  gWindow.OnMouseMove(
    static_cast<int>(p.x),
    static_cast<int>(p.y)
  );
}

- (void)mouseEntered:(NSEvent*)event {
  gWindow.OnMouseIn();
  [self sendMouseMoveFromEvent:event];
}

- (void)mouseExited:(NSEvent*)event {
  (void)event;

  gWindow.OnMouseOut();
  gWindow.OnMouseMove(-1, -1);
}

- (void)mouseMoved:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
}

- (void)mouseDragged:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
}

- (void)rightMouseDragged:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
}

- (void)otherMouseDragged:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
}

- (void)mouseDown:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
  gWindow.OnMouseDown(MouseButton::Left);
}

- (void)mouseUp:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
  gWindow.OnMouseUp(MouseButton::Left);
}

- (void)rightMouseDown:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
  gWindow.OnMouseDown(MouseButton::Right);
}

- (void)rightMouseUp:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];
  gWindow.OnMouseUp(MouseButton::Right);
}

- (void)otherMouseDown:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];

  if (event.buttonNumber == 2) {
    gWindow.OnMouseDown(MouseButton::Middle);
  }
}

- (void)otherMouseUp:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];

  if (event.buttonNumber == 2) {
    gWindow.OnMouseUp(MouseButton::Middle);
  }
}

- (void)scrollWheel:(NSEvent*)event {
  [self sendMouseMoveFromEvent:event];

  gWindow.OnMouseWheel(
    static_cast<float>(event.scrollingDeltaX),
    static_cast<float>(event.scrollingDeltaY),
    event.hasPreciseScrollingDeltas == YES);
}

- (void)keyDown:(NSEvent*)event {
  //if (gMacKeyboard != nullptr)
  //  gMacKeyboard->HandleKeyDown(event);
}

- (void)keyUp:(NSEvent*)event {
  //if (gMacKeyboard != nullptr)
  //  gMacKeyboard->HandleKeyUp(event);
}

@end

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>

@property(strong, nonatomic) NSWindow* window;
@property(strong, nonatomic) MacMetalHostView* metalView;
@property(strong, nonatomic) NSTimer* displayTimer;
@property(strong, nonatomic) id<MTLDevice> device;

@property(nonatomic, assign) NVGcontext* ctx;
@property(nonatomic, assign) NSSize lastViewSize;
@property(nonatomic, assign) CGFloat lastViewScale;
@property(nonatomic, assign) CFTimeInterval lastFrameTimestamp;

@end

@implementation AppDelegate

- (void)createWindow {
  int initialWidth = 1280;
  int initialHeight = 720;
  const char* windowTitle = "DirectUI";

  if (gConfig != nullptr) {
    if (gConfig->initialWidth > 0) {
      initialWidth = gConfig->initialWidth;
    }

    if (gConfig->initialHeight > 0) {
      initialHeight = gConfig->initialHeight;
    }

    if (!gConfig->windowTitle.empty()) {
      windowTitle = gConfig->windowTitle.c_str();
    }
  }

  NSRect frame = NSMakeRect(0.0, 0.0, initialWidth, initialHeight);

  NSWindowStyleMask styleMask =
    NSWindowStyleMaskTitled |
    NSWindowStyleMaskClosable |
    NSWindowStyleMaskMiniaturizable |
    NSWindowStyleMaskResizable;

  self.window = [[NSWindow alloc] initWithContentRect:frame
                                            styleMask:styleMask
                                              backing:NSBackingStoreBuffered
                                                defer:NO];

  self.window.delegate = self;
  self.window.title = [NSString stringWithUTF8String:windowTitle];

  self.metalView = [[MacMetalHostView alloc] initWithFrame:frame];
  self.metalView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

  self.window.contentView = self.metalView;
  [self.window center];
  [self.window makeKeyAndOrderFront:nil];

  [NSApp activateIgnoringOtherApps:YES];

  self.lastViewSize = NSMakeSize(0.0, 0.0);
  self.lastViewScale = -1.0;
}

- (BOOL)createMetalDevice {
  self.device = MTLCreateSystemDefaultDevice();

  NSLog(@"Metal device = %@", self.device);

  if (!self.device) {
    NSLog(@"No Metal device available.");
    return NO;
  }

  return YES;
}

- (void)configureMetalLayer {
  CAMetalLayer* layer = (CAMetalLayer*)self.metalView.layer;

  layer.device = self.device;
  layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  layer.framebufferOnly = NO;
  layer.opaque = YES;

  [self updateDrawableSize];
}

- (void)updateDrawableSize {
  CAMetalLayer* layer = (CAMetalLayer*)self.metalView.layer;

  CGFloat realScale = self.window != nil
    ? self.window.backingScaleFactor
    : [NSScreen mainScreen].backingScaleFactor;

  if (realScale <= 0.0) {
    realScale = 1.0;
  }

  CGFloat drawableScale = realScale;
  CGFloat directUIScale = IsNormalScaleEnabled() ? realScale : 1.0;

  NSSize size = self.metalView.bounds.size;
  NSSize drawableSize = NSMakeSize(
    size.width * drawableScale,
    size.height * drawableScale
  );

  NSSize directUISize = IsNormalScaleEnabled()
    ? size
    : NSMakeSize(
      size.width * realScale,
      size.height * realScale
    );

  if (NSEqualSizes(size, self.lastViewSize) &&
      std::fabs(realScale - self.lastViewScale) < 0.001f) {
    return;
  }

  self.lastViewSize = size;
  self.lastViewScale = realScale;

  layer.contentsScale = drawableScale;
  layer.drawableSize = drawableSize;

  gWindow.render_scale = static_cast<float>(directUIScale);
  gWindow.SetSize(
    static_cast<float>(directUISize.width),
    static_cast<float>(directUISize.height)
  );

  SizeRootControlToWindow();
}

- (BOOL)createDirectUIRenderTarget {
  if (gConfig == nullptr || gConfig->createRootControl == nullptr) {
    NSLog(@"DUIMacOSMetalHost missing root control callback.");
    return NO;
  }

  gRootControl = gConfig->createRootControl(gWindow, Platform::Instance);

  if (gRootControl == nullptr) {
    NSLog(@"DUIMacOSMetalHost root control callback returned null.");
    return NO;
  }

  gWindow.AddBorrowedControl(gRootControl);
  SizeRootControlToWindow();

  CAMetalLayer* layer = (CAMetalLayer*)self.metalView.layer;

  int nvgFlags = NVG_ANTIALIAS | NVG_STENCIL_STROKES;
#ifndef NDEBUG
  nvgFlags |= NVG_DEBUG;
#endif

  if (!gRenderTargetOwner.Create((__bridge void*)layer, nvgFlags)) {
    const char* metalError = mnvgLastError();
    const char* nvgError = nvgLastError();

    NSLog(@"NanoVG context creation failed. Metal: %s, NanoVG: %s",
          metalError ? metalError : "unknown",
          nvgError ? nvgError : "unknown");
    return NO;
  }

  gWindow.create_render_target(gRenderTargetOwner.Get());

  if (gWindow.graphics == nullptr) {
    NSLog(@"DirectUI graphics object is null.");
    return NO;
  }

  self.ctx = gRenderTargetOwner.Get();

  NSLog(@"NanoVG context created successfully. Target: %d", (int)mnvgTarget());
  return YES;
}

- (void)loadFonts {
  if (gConfig == nullptr ||
      gConfig->loadFonts == nullptr ||
      self.ctx == nullptr) {
    return;
  }

  gConfig->loadFonts(self.ctx);
}

- (void)startDisplayTimer {
  self.lastFrameTimestamp = 0.0;

  const int fps = PreferredFramesPerSecond();
  const NSTimeInterval frameInterval = 1.0 / static_cast<NSTimeInterval>(fps);

  self.displayTimer = [NSTimer timerWithTimeInterval:frameInterval
                                               target:self
                                             selector:@selector(renderFrame:)
                                             userInfo:nil
                                              repeats:YES];

  [[NSRunLoop mainRunLoop] addTimer:self.displayTimer
                            forMode:NSRunLoopCommonModes];
}

- (void)stopDisplayTimer {
  [self.displayTimer invalidate];
  self.displayTimer = nil;
}

- (void)renderFrame:(NSTimer*)timer {
  (void)timer;

  [self updateDrawableSize];

  if (self.ctx == nullptr) {
    return;
  }

  NSSize size = self.metalView.bounds.size;
  CGFloat pixelRatio = ((CAMetalLayer*)self.metalView.layer).contentsScale;

  if (size.width <= 0 || size.height <= 0 || pixelRatio <= 0) {
    return;
  }

  float deltaTime = 1.0f / 120.0f;

  const CFTimeInterval now = CFAbsoluteTimeGetCurrent();
  if (self.lastFrameTimestamp > 0.0) {
    const CFTimeInterval rawDelta = now - self.lastFrameTimestamp;
    if (rawDelta > 0.0) {
      deltaTime = static_cast<float>(rawDelta);
    }
  }

  self.lastFrameTimestamp = now;

  constexpr float maxDeltaTime = 1.0f / 15.0f;
  deltaTime = std::clamp(deltaTime, 0.0f, maxDeltaTime);

  mnvgClearWithColor(self.ctx, nvgRGBAf(0, 0, 0, 1.0f));
  gWindow.WindowRender(deltaTime);
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  (void)notification;

  Platform::Instance = gConfig != nullptr ? gConfig->platform : Platform{};

  [self createWindow];

  CGFloat runtimeScale = self.window != nil
    ? self.window.backingScaleFactor
    : 1.0;

  if (runtimeScale > 0.0) {
    Platform::Instance.scaleFactor = static_cast<float>(runtimeScale);
  }

  Platform::Instance.Print();

  RegisterHapticsIfConfigured();

  if (![self createMetalDevice]) {
    return;
  }

  [self configureMetalLayer];

  if ([self createDirectUIRenderTarget]) {
    [self loadFonts];
  }

  [self startDisplayTimer];
}

- (void)applicationWillTerminate:(NSNotification*)notification {
  (void)notification;

  [self stopDisplayTimer];

  gRenderTargetOwner.Reset();
  self.ctx = nullptr;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
  (void)sender;
  return YES;
}

@end

int DUIRunMacOSMetalHost(
  int argc,
  char** argv,
  DUIMacOSMetalHostConfig config
) {
  (void)argc;
  (void)argv;

  gConfig = std::make_unique<DUIMacOSMetalHostConfig>(std::move(config));

  RedirectLogsIfEnabled();
  RegisterStylesIfConfigured();
  ConfigureWindowIfConfigured();

  @autoreleasepool {
    NSApplication* app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    AppDelegate* delegate = [AppDelegate new];
    [app setDelegate:delegate];

    [app run];
  }

  return EXIT_SUCCESS;
}

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <kromakit/platform/ios/DUIIOSMetalHost.h>

#include <fcntl.h>
#include <unistd.h>

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

#include <kromakit/TextInputBridge.h>

namespace {



std::unique_ptr<DUIIOSMetalHostConfig> gConfig;

DUIWindow gWindow;
Control* gRootControl = nullptr;
MetalNanoVGContextOwner gRenderTargetOwner;

void SizeRootControlToWindow() {
  if (gRootControl == nullptr) {
    return;
  }

  gRootControl->SetLayoutLocation({0.0f, 0.0f});
  gRootControl->SetLayoutSize(gWindow.size);
}

bool IsNormalScaleEnabled() {
  return gConfig == nullptr || gConfig->normalScale;
}

int PreferredFramesPerSecond() {
  if (gConfig == nullptr || gConfig->preferredFramesPerSecond <= 0) {
    return 120;
  }

  return gConfig->preferredFramesPerSecond;
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

  // DUIWindow is a static global in this host. It is constructed before
  // registerStyles executes, so internal stacks may miss their initial style
  // lookup. Re-apply after registration to keep runtime parity.
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

@interface DUIHiddenTextField : UITextField
@property(nonatomic, copy) void (^onDeleteBackward)(void);
@end

@implementation DUIHiddenTextField

- (void)deleteBackward {
  if (self.onDeleteBackward != nil) {
    self.onDeleteBackward();
  }

  [super deleteBackward];
}



@end

@interface MetalHostView : UIView <UITextFieldDelegate>
@property(nonatomic, strong) DUIHiddenTextField* hiddenTextField;
@end

@implementation MetalHostView

- (void)setupHiddenTextField {
  self.hiddenTextField = [[DUIHiddenTextField alloc] initWithFrame:CGRectMake(0, 0, 1, 1)];

  self.hiddenTextField.alpha = 0.01;
  self.hiddenTextField.delegate = self;

  self.hiddenTextField.autocorrectionType = UITextAutocorrectionTypeNo;
  self.hiddenTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
  self.hiddenTextField.spellCheckingType = UITextSpellCheckingTypeNo;
  self.hiddenTextField.keyboardType = UIKeyboardTypeDefault;
  self.hiddenTextField.returnKeyType = UIReturnKeyDone;

  __weak MetalHostView* weakSelf = self;

  self.hiddenTextField.onDeleteBackward = ^{
    (void)weakSelf;

    DUIKeyEvent event;
    event.key = DUIKey::Backspace;
    event.pressed = true;

    gWindow.OnKeyDown(event);
  };

  [self addSubview:self.hiddenTextField];
}

- (BOOL)textFieldShouldReturn:(UITextField*)textField {
  (void)textField;

  [self hideKeyboard];
  return NO;
}

- (void)showKeyboard {
  [self.hiddenTextField becomeFirstResponder];
}

- (void)hideKeyboard {
  [self.hiddenTextField resignFirstResponder];
}

- (BOOL)textField:(UITextField*)textField
shouldChangeCharactersInRange:(NSRange)range
replacementString:(NSString*)string {
  (void)textField;
  (void)range;

  if ([string isEqualToString:@"\n"]) {
    [self hideKeyboard];
    return NO;
  }

  if (string.length == 0) {
    DUIKeyEvent event;
    event.key = DUIKey::Backspace;
    event.pressed = true;

    gWindow.OnKeyDown(event);
    return NO;
  }

  for (NSUInteger i = 0; i < string.length; i++) {
    unichar ch = [string characterAtIndex:i];

    DUITextInputEvent event;
    event.codepoint = static_cast<char32_t>(ch);

    gWindow.OnTextInput(event);
  }

  return NO;
}



- (void)dealloc {
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)startKeyboardObservers {
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(onKeyboardWillChangeFrame:)
                                               name:UIKeyboardWillChangeFrameNotification
                                             object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(onKeyboardWillHide:)
                                               name:UIKeyboardWillHideNotification
                                             object:nil];
}

- (void)onKeyboardWillChangeFrame:(NSNotification*)notification {
  CGRect keyboardScreenFrame =
    [notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];

  CGRect keyboardViewFrame =
    [self convertRect:keyboardScreenFrame fromView:nil];

  CGRect overlap =
    CGRectIntersection(self.bounds, keyboardViewFrame);

  BOOL visible =
    !CGRectIsNull(overlap) &&
    overlap.size.height > 0.0;

  DUIRect frame = {
    static_cast<float>(overlap.origin.x),
    static_cast<float>(overlap.origin.y),
    static_cast<float>(overlap.size.width),
    static_cast<float>(overlap.size.height)
  };

  if (gConfig->onKeyboardFrameChanged) {
    gConfig->onKeyboardFrameChanged(
      visible,
      frame
    );
  }
}

- (void)onKeyboardWillHide:(NSNotification*)notification {
  DUIRect frame = {0, 0, 0, 0};

  if (gConfig->onKeyboardFrameChanged) {
    gConfig->onKeyboardFrameChanged(
      false,
      frame
    );
  }
}

- (CGFloat)directUIInputScale {
  CGFloat realScale = self.window.screen
    ? self.window.screen.scale
    : UIScreen.mainScreen.scale;

  return IsNormalScaleEnabled() ? 1.0 : realScale;
}

- (CGPoint)directUIPointFromUIKitPoint:(CGPoint)p {
  CGFloat inputScale = [self directUIInputScale];

  return CGPointMake(
    p.x * inputScale,
    p.y * inputScale
  );
}

+ (Class)layerClass {
  return [CAMetalLayer class];
}

- (CGPoint)primaryTouchPointFromTouches:(NSSet<UITouch*>*)touches {
  UITouch* touch = touches.anyObject;
  return [touch locationInView:self];
}

- (void)sendTouchMoveToDirectUI:(CGPoint)p {
  CGPoint directUIPoint = [self directUIPointFromUIKitPoint:p];

  gWindow.OnMouseMove(
    static_cast<float>(directUIPoint.x),
    static_cast<float>(directUIPoint.y)
  );
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
  (void)event;

  CGPoint p = [self primaryTouchPointFromTouches:touches];

  [self sendTouchMoveToDirectUI:p];
  gWindow.OnMouseDown(MouseButton::Left);
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
  (void)event;

  CGPoint p = [self primaryTouchPointFromTouches:touches];
  [self sendTouchMoveToDirectUI:p];
}

- (void)finishTouchAtPoint:(CGPoint)p {
  [self sendTouchMoveToDirectUI:p];
  gWindow.OnMouseUp(MouseButton::Left);

  // Move cursor away after release so hover/pressed state behaves more mobile-like.
  gWindow.OnMouseMove(-1.0f, -1.0f);
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
  (void)event;

  CGPoint p = [self primaryTouchPointFromTouches:touches];
  [self finishTouchAtPoint:p];
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
  (void)event;

  CGPoint p = [self primaryTouchPointFromTouches:touches];
  [self finishTouchAtPoint:p];
}

@end


@interface RootViewController : UIViewController
@property(nonatomic, weak) MetalHostView* metalView;
@end

@implementation RootViewController

- (void)viewDidLayoutSubviews {
  [super viewDidLayoutSubviews];

  UIEdgeInsets insets = self.view.safeAreaInsets;

  CGFloat realScale = self.view.window.screen
    ? self.view.window.screen.scale
    : UIScreen.mainScreen.scale;

  CGFloat directUIScale = IsNormalScaleEnabled() ? 1.0 : realScale;

  gWindow.safeArea = {
    static_cast<float>(insets.left * directUIScale),
    static_cast<float>(insets.top * directUIScale),
    static_cast<float>(insets.right * directUIScale),
    static_cast<float>(insets.bottom * directUIScale)
  };
}

@end


@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property(strong, nonatomic) UIWindow* window;
@property(strong, nonatomic) MetalHostView* metalView;
@property(strong, nonatomic) CADisplayLink* displayLink;
@property(strong, nonatomic) id<MTLDevice> device;

@property(nonatomic, assign) NVGcontext* ctx;
@property(nonatomic, assign) NSInteger frameCounter;
@property(nonatomic, assign) CGSize lastDrawableSize;
@property(nonatomic, assign) CGFloat lastDrawableScale;

@property (nonatomic, assign) CFTimeInterval lastFrameTimestamp;

@end

@implementation AppDelegate



- (void)createWindow {
  CGRect bounds = UIScreen.mainScreen.bounds;

  self.window = [[UIWindow alloc] initWithFrame:bounds];

  RootViewController* rootVC = [RootViewController new];
  rootVC.view = [[UIView alloc] initWithFrame:bounds];
  rootVC.view.backgroundColor = UIColor.blackColor;

  self.metalView = [[MetalHostView alloc] initWithFrame:bounds];
  self.metalView.userInteractionEnabled = YES;
  self.metalView.autoresizingMask =
    UIViewAutoresizingFlexibleWidth |
    UIViewAutoresizingFlexibleHeight;

  [self.metalView startKeyboardObservers];
  [self.metalView setupHiddenTextField];

  MetalHostView* metalView = self.metalView;

  DirectUITextInput::Register(
    [metalView]() {
      [metalView showKeyboard];
    },
    [metalView]() {
      [metalView hideKeyboard];
    }
  );

  [rootVC.view addSubview:self.metalView];
  rootVC.metalView = self.metalView;

  self.window.rootViewController = rootVC;
  [self.window makeKeyAndVisible];

  self.lastDrawableSize = CGSizeMake(0.0, 0.0);
  self.lastDrawableScale = -1.0;
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

  CGFloat realScale = self.window.screen
    ? self.window.screen.scale
    : UIScreen.mainScreen.scale;

  CGFloat drawableScale = IsNormalScaleEnabled() ? realScale : realScale;
  CGFloat directUIScale = IsNormalScaleEnabled() ? realScale : 1.0;

  CGSize size = self.metalView.bounds.size;

  CGSize drawableSize = CGSizeMake(
    size.width * drawableScale,
    size.height * drawableScale
  );

  CGSize directUISize = IsNormalScaleEnabled()
    ? size
    : CGSizeMake(
      size.width * realScale,
      size.height * realScale
    );

  if (CGSizeEqualToSize(size, self.lastDrawableSize) &&
      realScale == self.lastDrawableScale) {
    return;
  }

  self.lastDrawableSize = size;
  self.lastDrawableScale = realScale;

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
    NSLog(@"DUIIOSMetalHost missing root control callback.");
    return NO;
  }

  gRootControl = gConfig->createRootControl(gWindow, Platform::Instance);

  if (gRootControl == nullptr) {
    NSLog(@"DUIIOSMetalHost root control callback returned null.");
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

- (void)startDisplayLink {
  self.lastFrameTimestamp = 0.0;

  self.displayLink = [CADisplayLink displayLinkWithTarget:self
                                                  selector:@selector(renderFrame:)];

  self.displayLink.preferredFramesPerSecond = PreferredFramesPerSecond();

  [self.displayLink addToRunLoop:NSRunLoop.mainRunLoop
                         forMode:NSRunLoopCommonModes];
}

- (void)stopDisplayLink {
  [self.displayLink invalidate];
  self.displayLink = nil;
}

- (void)renderFrame:(CADisplayLink*)displayLink {
  self.frameCounter += 1;

  [self updateDrawableSize];

  if (self.ctx == nullptr) {
    return;
  }

  CGSize size = self.metalView.bounds.size;
  CGFloat pixelRatio = ((CAMetalLayer*)self.metalView.layer).contentsScale;

  if (size.width <= 0 || size.height <= 0 || pixelRatio <= 0) {
    return;
  }

  float deltaTime = 1.0f / 120.0f;

  if (self.lastFrameTimestamp > 0.0) {
    const CFTimeInterval rawDelta =
      displayLink.timestamp - self.lastFrameTimestamp;

    if (rawDelta > 0.0) {
      deltaTime = static_cast<float>(rawDelta);
    }
  }

  self.lastFrameTimestamp = displayLink.timestamp;

  // Avoid one giant physics/layout step after app stalls, debugger pauses,
  // backgrounding, etc.
  constexpr float maxDeltaTime = 1.0f / 15.0f;
  if (deltaTime > maxDeltaTime) {
    deltaTime = maxDeltaTime;
  }

  mnvgClearWithColor(self.ctx, nvgRGBAf(0, 0, 0, 1.0f));

  gWindow.WindowRender(deltaTime);
}

- (BOOL)application:(UIApplication*)application
didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
  (void)application;
  (void)launchOptions;

  Platform::Instance = gConfig != nullptr ? gConfig->platform : Platform{};
  Platform::Instance.Print();

  RegisterHapticsIfConfigured();

  [self createWindow];

  if (![self createMetalDevice]) {
    return YES;
  }

  [self configureMetalLayer];

  if ([self createDirectUIRenderTarget]) {
    [self loadFonts];
  }

  [self startDisplayLink];

  return YES;
}

- (void)applicationWillTerminate:(UIApplication*)application {
  (void)application;

  [self stopDisplayLink];

  gRenderTargetOwner.Reset();
  self.ctx = nullptr;
}

@end

int DUIRunIOSMetalHost(int argc, char** argv, DUIIOSMetalHostConfig config) {
  gConfig = std::make_unique<DUIIOSMetalHostConfig>(std::move(config));

  RedirectLogsIfEnabled();
  RegisterStylesIfConfigured();
  ConfigureWindowIfConfigured();

  @autoreleasepool {
    return UIApplicationMain(
      argc,
      argv,
      nil,
      NSStringFromClass([AppDelegate class])
    );
  }
}

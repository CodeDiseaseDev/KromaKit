
#import <UIKit/UIKit.h>


#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/NullFilePicker.h>
#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/apple/AppleMainThreadDispatcher.h>

#include <memory>

namespace {
  UIViewController* GetTopViewController() {
    UIWindow* window = UIApplication.sharedApplication.keyWindow;

    if (window == nil) {
      for (UIWindow* candidate in UIApplication.sharedApplication.windows) {
        if (candidate.isKeyWindow) {
          window = candidate;
          break;
        }
      }
    }

    UIViewController* controller = window.rootViewController;

    while (controller.presentedViewController != nil) {
      controller = controller.presentedViewController;
    }

    return controller;
  }
}

void PlatformServices::DisplayMessageBox_(
  std::string title,
  std::string message
) {
  NSString* nsTitle =
    [NSString stringWithUTF8String:title.c_str()];

  NSString* nsMessage =
    [NSString stringWithUTF8String:message.c_str()];

  dispatch_async(dispatch_get_main_queue(), ^{
    UIViewController* controller = GetTopViewController();

    if (controller == nil) {
      return;
    }

    UIAlertController* alert =
      [UIAlertController alertControllerWithTitle:nsTitle ?: @"Message"
                                          message:nsMessage ?: @""
                                   preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok =
      [UIAlertAction actionWithTitle:@"OK"
                               style:UIAlertActionStyleDefault
                             handler:nil];

    [alert addAction:ok];

    [controller presentViewController:alert
                             animated:YES
                           completion:nil];
  });
}

std::shared_ptr<IFilePicker> PlatformServices::FilePicker() {
  static std::shared_ptr<IFilePicker> picker =
    std::make_shared<NullFilePicker>();

  return picker;
}



std::shared_ptr<IMainThreadDispatcher>
PlatformServices::MainThreadDispatcher() {
  static std::shared_ptr<IMainThreadDispatcher> dispatcher =
    std::make_shared<AppleMainThreadDispatcher>();

  return dispatcher;
}

void PlatformServices::PostToMainThread(
  std::function<void()> action
) {
  MainThreadDispatcher()->Post(std::move(action));
}

void PlatformServices::DispatchOrRunOnMainThread(
  std::function<void()> action
) {
  MainThreadDispatcher()->DispatchOrRun(std::move(action));
}

void PlatformServices::DrainMainThreadTasks() {
  MainThreadDispatcher()->Drain();
}


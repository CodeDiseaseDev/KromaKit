//
// Created by code on 6/2/26.
//

#include <kromakit/platform/apple/AppleMainThreadDispatcher.h>

#include <dispatch/dispatch.h>
#include <memory>
#include <pthread.h>

bool AppleMainThreadDispatcher::IsMainThread() const {
  return pthread_main_np() != 0;
}

void AppleMainThreadDispatcher::Post(
  std::function<void()> action
) {
  if (!action) {
    return;
  }

  auto actionPtr =
    std::make_shared<std::function<void()>>(std::move(action));

  dispatch_async(dispatch_get_main_queue(), ^{
    if (*actionPtr) {
      (*actionPtr)();
    }
  });
}

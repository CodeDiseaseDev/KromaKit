//
// Created by code on 6/2/26.
//

#ifndef KROMAKIT_APPLEMAINTHREADDISPATCHER_H
#define KROMAKIT_APPLEMAINTHREADDISPATCHER_H

#if defined(__APPLE__)

#include <kromakit/platform/IMainThreadDispatcher.h>

#include <functional>

class AppleMainThreadDispatcher :
  public IMainThreadDispatcher {
public:
  bool IsMainThread() const override;

  void Post(
    std::function<void()> action
  ) override;
};

#endif

#endif // KROMAKIT_APPLEMAINTHREADDISPATCHER_H

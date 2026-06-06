//
// Created by code on 6/2/26.
//

#ifndef KROMAKIT_IMAINTHREADDISPATCHER_H
#define KROMAKIT_IMAINTHREADDISPATCHER_H

#include <functional>

class IMainThreadDispatcher {
public:
  virtual ~IMainThreadDispatcher() = default;

  virtual bool IsMainThread() const = 0;

  virtual void Post(
    std::function<void()> action
  ) = 0;

  virtual void Drain() {
  }

  void DispatchOrRun(
    std::function<void()> action
  ) {
    if (IsMainThread()) {
      action();
      return;
    }

    Post(std::move(action));
  }
};

#endif // KROMAKIT_IMAINTHREADDISPATCHER_H
//
// Created by code on 6/2/26.
//

#ifndef KROMAKIT_QUEUEDMAINTHREADDISPATCHER_H
#define KROMAKIT_QUEUEDMAINTHREADDISPATCHER_H

#include <kromakit/platform/IMainThreadDispatcher.h>

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

class QueuedMainThreadDispatcher : public IMainThreadDispatcher {
public:
  QueuedMainThreadDispatcher()
    : mainThreadId(std::this_thread::get_id()) {
  }

  bool IsMainThread() const override {
    return std::this_thread::get_id() == mainThreadId;
  }

  void Post(
    std::function<void()> action
  ) override {
    if (!action) {
      return;
    }

    {
      std::lock_guard<std::mutex> lock(mutex);
      pendingActions.push_back(std::move(action));
    }

    Wake();
  }

  void Drain() override {
    if (!IsMainThread()) {
      return;
    }

    std::vector<std::function<void()>> actions;

    {
      std::lock_guard<std::mutex> lock(mutex);
      actions.swap(pendingActions);
    }

    for (auto& action : actions) {
      if (action) {
        action();
      }
    }
  }

protected:
  virtual void Wake() {
    // Optional platform-specific event-loop wake.
    // For now, the next frame will drain queued actions.
  }

private:
  std::thread::id mainThreadId;
  mutable std::mutex mutex;
  std::vector<std::function<void()>> pendingActions;
};

#endif // KROMAKIT_QUEUEDMAINTHREADDISPATCHER_H
#pragma once

#include <functional>
#include <memory>

#include <kromakit/gesture/hold/HoldInteraction.h>

class ContextMenu;

class ContextMenuAttachment {
public:
  using Provider = std::function<std::unique_ptr<ContextMenu>()>;

  static constexpr int kDefaultHoldThresholdMs = 450;

  ContextMenuAttachment() = default;

  void SetProvider(Provider provider) {
    provider_ = std::move(provider);
  }

  void ClearProvider() {
    provider_ = nullptr;
    holdInteraction_.Cancel();
  }

  bool HasProvider() const {
    return static_cast<bool>(provider_);
  }

  const Provider& GetProvider() const {
    return provider_;
  }

  void SetHoldThresholdMs(int milliseconds) {
    holdInteraction_.SetHoldThresholdMs(milliseconds);
  }

  int GetHoldThresholdMs() const {
    return holdInteraction_.GetHoldThresholdMs();
  }

  HoldInteraction& HoldInteractionState() {
    return holdInteraction_;
  }

  const HoldInteraction& HoldInteractionState() const {
    return holdInteraction_;
  }

private:
  Provider provider_ = nullptr;
  HoldInteraction holdInteraction_ {};
};

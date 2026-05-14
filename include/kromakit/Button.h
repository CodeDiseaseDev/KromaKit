#pragma once
#include <kromakit/Control.h>
#include <functional>

class Button :
    public Control
{
public:
  std::function<void()> OnClick;

  void OnRender(Graphics* rendTarget) override;
  void DoLayout(Graphics* renderTarget) override;

  void OnMouseDown() override;
  void OnMouseUp() override;

  void OnMouseIn() override;
  void OnMouseOut() override;

  void OnIsSelectedChanged() override;

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;

  bool WantsClickCapture() const override;

  Button();
  Button(std::string content);
};


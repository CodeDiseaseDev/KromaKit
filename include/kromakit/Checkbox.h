#pragma once
#include <kromakit/Control.h>
#include <kromakit/Label.h>
#include <functional>

class Checkbox :
    public Control
{
  float spacing = 5.0f;
  float selected_circle_indic_rad = 11.0f;

  Label* checkboxLabel;

public:

  bool isChecked = false;

  std::function<void(bool)> OnCheckedChanged;

  Checkbox();

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;

  void OnRender(Graphics* rendTarget) override;

  bool WantsClickCapture() const;

  void DoLayout(Graphics* renderTarget) override;
  void OnMouseDown() override;
  void SetText(std::string str);
};


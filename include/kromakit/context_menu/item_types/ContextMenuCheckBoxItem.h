//
// Created by code on 5/28/26.
//

#ifndef TEST_IOS_APP_CONTEXTMENUCHECKBOXITEM_H
#define TEST_IOS_APP_CONTEXTMENUCHECKBOXITEM_H

#include <kromakit/context_menu/IContextMenuItem.h>
#include <kromakit/Checkbox.h>
#include <kromakit/Label.h>

#include <functional>
#include <optional>
#include <string>
#include <vector>

class ContextMenuCheckBoxItem :
  public IContextMenuItem
{
public:
  std::function<void(bool)> OnCheckedChanged;

  ContextMenuCheckBoxItem();
  explicit ContextMenuCheckBoxItem(std::string text);
  ContextMenuCheckBoxItem(std::string text, bool checked);

  void SetLabelText(const std::string& text);
  void SetChecked(bool checked, bool notify = false);

  [[nodiscard]] bool IsChecked() const;

  void DoLayout(Graphics* renderTarget) override;
  void OnRender(Graphics* rendTarget) override;

  void OnPointerUp(const PointerEvent &e) override;

  std::optional<std::string> GetControlName() const override {
    return "ContextMenuCheckBoxItem";
  }

  const std::vector<std::string> GetDebugOverlayItems() override {
    return {};
  }

private:
  Checkbox* checkbox = nullptr;
  // Label* label = nullptr;

  bool isChecked = false;

  void Toggle();
};

#endif // TEST_IOS_APP_CONTEXTMENUCHECKBOXITEM_H
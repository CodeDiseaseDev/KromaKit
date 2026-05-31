//
// Created by code on 5/31/26.
//

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
#include "MessageBoxOptions.h"
#include "kromakit/Label.h"
#include "kromakit/Panel.h"

#include "kromakit/overlay_background_layer/IOverlayControl.h"


class MessageBox :
  public IOverlayControl {

public:
  MessageBox();
  explicit MessageBox(MessageBoxOptions options);

  void OnOverlayShown(OverlayBackgroundLayer* layer) override;
  void OnOverlayDismissed() override;

  bool ShouldDismissOnBackgroundTap() const override;

  void SetOptions(MessageBoxOptions options);

  static MessageBoxOptions Info(
    std::string title,
    std::string message);

  static MessageBoxOptions Error(
    std::string title,
    std::string message,
    std::string details = "");

  static MessageBoxOptions Confirm(
    std::string title,
    std::string message,
    std::function<void(bool)> onResult);

private:
  MessageBoxOptions options_;

  Panel* surface_ = nullptr;
  Label* titleLabel_ = nullptr;
  Label* messageLabel_ = nullptr;
  Label* detailsLabel_ = nullptr;
  Button* detailsButton_ = nullptr;
  Panel* buttonPanel_ = nullptr;

  bool detailsVisible_ = false;

  void BuildUI();
  void RebuildButtons();

  void Complete(MessageBoxResult result);
  void ToggleDetails();

  static void EnsureDefaultButtons(MessageBoxOptions& options);
};


#endif //MESSAGEBOX_H

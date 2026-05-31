#include "kromakit/message_box/MessageBox.h"

#include <utility>

#include "kromakit/Button.h"
#include "kromakit/Label.h"
#include "kromakit/Panel.h"
#include "kromakit/overlay_background_layer/OverlayBackgroundLayer.h"

MessageBox::MessageBox() = default;

MessageBox::MessageBox(MessageBoxOptions options)
  : options_(std::move(options)) {

  EnsureDefaultButtons(options_);

  detailsVisible_ = options_.showDetailsByDefault;

  BuildUI();
}

void MessageBox::EnsureDefaultButtons(MessageBoxOptions& options) {
  if (!options.buttons.empty()) {
    return;
  }

  options.buttons.push_back({
    .text = "OK",
    .result = MessageBoxResult::Ok,
    .isDefault = true,
    .isCancel = true
  });
}

void MessageBox::BuildUI() {
  // Replace this with your actual DUI/KromaKit layout API.
  // This is intentionally boring: MessageBox should own layout only,
  // not overlay lifetime.

  surface_ = CreateControl<Panel>();

  titleLabel_ = surface_->CreateControl<Label>(options_.title);
  messageLabel_ = surface_->CreateControl<Label>(options_.message);

  if (!options_.details.empty()) {
    detailsButton_ = surface_->CreateControl<Button>(
      detailsVisible_ ? "Hide details" : "Show details");

    detailsButton_->OnClick = [this] {
      ToggleDetails();
    };

    detailsLabel_ = surface_->CreateControl<Label>(options_.details);

    // Use whatever visibility API your framework has.
    detailsLabel_->isVisible = detailsVisible_;
  }

  buttonPanel_ = surface_->CreateControl<Panel>();

  RebuildButtons();

  MarkLayoutDirty();
}

void MessageBox::RebuildButtons() {
  if (buttonPanel_ == nullptr) {
    return;
  }

  // Replace with your framework's actual child clearing method.
  buttonPanel_->Children.clear();

  for (auto& buttonOptions : options_.buttons) {
    auto* button = buttonPanel_->CreateControl<Button>(buttonOptions.text);

    button->OnClick = [this, result = buttonOptions.result] {
      Complete(result);
    };

    // Optional styling hooks for later:
    //
    // if (buttonOptions.destructive) {
    //   button->UseStyle(DangerButtonStyle);
    // }
    //
    // if (buttonOptions.isDefault) {
    //   button->UseStyle(DefaultButtonStyle);
    // }
  }

  MarkLayoutDirty();
}

void MessageBox::Complete(MessageBoxResult result) {
  for (auto& button : options_.buttons) {
    if (button.result == result && button.onClick) {
      button.onClick();
      break;
    }
  }

  if (options_.onResult) {
    options_.onResult(result);
  }

  DismissBackgroundLayer();
}

void MessageBox::ToggleDetails() {
  detailsVisible_ = !detailsVisible_;

  if (detailsButton_ != nullptr) {
    detailsButton_->SetContent(detailsVisible_
      ? "Hide details"
      : "Show details");
  }

  if (detailsLabel_ != nullptr) {
    detailsLabel_->isVisible = detailsVisible_;
  }

  MarkLayoutDirty();
}

void MessageBox::OnOverlayShown(OverlayBackgroundLayer* layer) {
  IOverlayControl::OnOverlayShown(layer);

  // Later:
  // - focus default button
  // - capture keyboard navigation
  // - enter = default button
  // - escape = cancel button
}

void MessageBox::OnOverlayDismissed() {
  IOverlayControl::OnOverlayDismissed();

  // Later:
  // - release keyboard capture
  // - clear transient animation state
}

bool MessageBox::ShouldDismissOnBackgroundTap() const {
  return options_.dismissOnBackgroundTap;
}

void MessageBox::SetOptions(MessageBoxOptions options) {
  options_ = std::move(options);

  EnsureDefaultButtons(options_);

  detailsVisible_ = options_.showDetailsByDefault;

  BuildUI();
  MarkLayoutDirty();
}

MessageBoxOptions MessageBox::Info(
  std::string title,
  std::string message) {

  MessageBoxOptions options;
  options.kind = MessageBoxKind::Info;
  options.title = std::move(title);
  options.message = std::move(message);
  options.dismissOnBackgroundTap = true;

  options.buttons.push_back({
    .text = "OK",
    .result = MessageBoxResult::Ok,
    .isDefault = true,
    .isCancel = true
  });

  return options;
}

MessageBoxOptions MessageBox::Error(
  std::string title,
  std::string message,
  std::string details) {

  MessageBoxOptions options;
  options.kind = MessageBoxKind::Error;
  options.title = std::move(title);
  options.message = std::move(message);
  options.details = std::move(details);
  options.dismissOnBackgroundTap = false;
  options.showDetailsByDefault = false;

  options.buttons.push_back({
    .text = "OK",
    .result = MessageBoxResult::Ok,
    .isDefault = true,
    .isCancel = true
  });

  return options;
}

MessageBoxOptions MessageBox::Confirm(
  std::string title,
  std::string message,
  std::function<void(bool)> onResult) {

  MessageBoxOptions options;
  options.kind = MessageBoxKind::Question;
  options.title = std::move(title);
  options.message = std::move(message);
  options.dismissOnBackgroundTap = false;

  options.buttons.push_back({
    .text = "Cancel",
    .result = MessageBoxResult::Cancel,
    .isCancel = true
  });

  options.buttons.push_back({
    .text = "Yes",
    .result = MessageBoxResult::Yes,
    .isDefault = true
  });

  options.onResult = [onResult = std::move(onResult)](MessageBoxResult result) {
    if (onResult) {
      onResult(result == MessageBoxResult::Yes);
    }
  };

  return options;
}
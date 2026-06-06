#include "kromakit/message_box/MessageBox.h"

#include <algorithm>
#include <utility>

#include "kromakit/Button.h"
#include "kromakit/Label.h"
#include "kromakit/Panel.h"
#include "kromakit/overlay_background_layer/OverlayBackgroundLayer.h"

#include <vector>

#include "kromakit/grid_layout_panel/GridLayoutPanel.h"

#include "kromakit/svg/SvgIcon.h"
#include <optional>

#include "kromakit/graphics/svg/FontAwesomeIcons.h"

namespace {
  struct MessageBoxIconStyle {
    std::string_view svgPath;
    Color tint;
  };

  constexpr float SurfaceWidth = 460.0f;
  constexpr float MinSurfaceHeight = 180.0f;

  constexpr float Padding = 12.0f;
  constexpr float Spacing = 10.0f;

  constexpr float TitleHeight = 28.0f;
  constexpr float MessageHeight = 56.0f;
  constexpr float DetailsButtonHeight = 30.0f;
  constexpr float DetailsHeight = 110.0f;
  constexpr float ButtonHeight = 36.0f;

  constexpr float ButtonWidth = 96.0f;
  constexpr float ButtonSpacing = 8.0f;

  constexpr float IconColumnWidth = 42.0f;
  constexpr float IconGap = 14.0f;
  constexpr float IconSize = 28.0f;

  constexpr int IconColumn = 0;
  constexpr int GapColumn = 1;
  constexpr int ContentColumn = 2;
  constexpr int ColumnCount = 3;

  const std::map<MessageBoxKind, MessageBoxIconStyle> MessageBoxIconStyles {
      {
        MessageBoxKind::Info,
        MessageBoxIconStyle{
          .svgPath = DUIIcons::FontAwesome::Solid::CircleInfo,
          .tint = ColorFromRGB(0, 120, 215)
        }
      },
      {
        MessageBoxKind::Error,
        MessageBoxIconStyle{
          .svgPath = DUIIcons::FontAwesome::Solid::TriangleExclamation,
          .tint = ColorFromRGB(255, 90, 80)
        }
      },
      {
        MessageBoxKind::Warning,
        MessageBoxIconStyle{
          .svgPath = DUIIcons::FontAwesome::Solid::TriangleExclamation,
          .tint = ColorFromRGB(255, 160, 80)
        }
      },
      {
        MessageBoxKind::Question,
        MessageBoxIconStyle{
          .svgPath = DUIIcons::FontAwesome::Solid::CircleQuestion,
          .tint = ColorFromRGB(70, 70, 255)
        }
      }
  };

  std::optional<MessageBoxIconStyle> GetMessageBoxIconStyle(
    MessageBoxKind kind
  ) {
    const auto it = MessageBoxIconStyles.find(kind);

    if (it == MessageBoxIconStyles.end()) {
      return std::nullopt;
    }

    return it->second;
  }

  float GetMessageBoxHeight(bool hasDetails, bool detailsVisible) {
    float fixedHeight =
      TitleHeight +
      Spacing +
      MessageHeight +
      Spacing +      // gap before buttons
      ButtonHeight;

    if (hasDetails) {
      fixedHeight += Spacing + DetailsButtonHeight;
    }

    if (hasDetails && detailsVisible) {
      fixedHeight += Spacing + DetailsHeight;
    }

    return std::max(
      MinSurfaceHeight,
      Padding * 2.0f + fixedHeight);
  }

  void ConfigureMessageBoxRows(
    GridLayoutPanel* grid,
    bool hasDetails,
    bool detailsVisible
  ) {
    if (grid == nullptr) {
      return;
    }

    grid->SetGridColumns({
      LayoutLength::Fixed(IconColumnWidth),
      LayoutLength::Fixed(IconGap),
      LayoutLength::Fill()
    });

    grid->SetGridRows({
      LayoutLength::Fixed(TitleHeight),
      LayoutLength::Fixed(Spacing),
      LayoutLength::Fixed(MessageHeight),

      LayoutLength::Fixed(hasDetails ? Spacing : 0.0f),
      LayoutLength::Fixed(hasDetails ? DetailsButtonHeight : 0.0f),

      LayoutLength::Fixed(
        hasDetails && detailsVisible ? Spacing : 0.0f),

      LayoutLength::Fixed(
        hasDetails && detailsVisible ? DetailsHeight : 0.0f),

      LayoutLength::Fixed(Spacing),
      LayoutLength::Fill(),
      LayoutLength::Fixed(ButtonHeight)
    });
  }


}


MessageBox::MessageBox() = default;

MessageBox::MessageBox(MessageBoxOptions options)
  : options_(std::move(options)) {

  EnsureDefaultButtons(options_);

  detailsVisible_ = options_.showDetailsByDefault;

  BuildUI();
}

bool MessageBox::ShouldBeCentered() {
  return true;
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
  Children.clear();
  ownedChildren.clear();

  surface_ = nullptr;
  layoutGrid_ = nullptr;
  titleLabel_ = nullptr;
  messageLabel_ = nullptr;
  detailsLabel_ = nullptr;
  detailsButton_ = nullptr;
  buttonPanel_ = nullptr;
  icon_ = nullptr;

  const bool hasDetails = !options_.details.empty();

  surface_ = CreateControl<Panel>();
  surface_->cornerRadius = DUIInsets(8);

  layoutGrid_ = surface_->CreateControl<GridLayoutPanel>();
  layoutGrid_->SetPadding(DUIInsets(Padding));

  ConfigureMessageBoxRows(
    layoutGrid_,
    hasDetails,
    detailsVisible_);

  icon_ = layoutGrid_->CreateLayoutChild<SvgIcon>(
  GridLayoutOptions{IconColumn, 0, 1, 1});

  icon_->SetLayoutFrame(
    {0.0f, 0.0f},
    {IconSize, IconSize});

  if (auto iconStyle = GetMessageBoxIconStyle(options_.kind)) {
    icon_->LoadSVG(iconStyle->svgPath);
    icon_->SetTint(iconStyle->tint);
    icon_->isVisible = true;
  }
  else {
    icon_->isVisible = false;
  }

  titleLabel_ = layoutGrid_->CreateLayoutChild<Label>(
    GridLayoutOptions{ContentColumn, 0, 1, 1});
  titleLabel_->SetContent(options_.title);
  titleLabel_->autoWidth = false;
  titleLabel_->autoHeight = false;
  titleLabel_->font.FontSize = 20;
  titleLabel_->font.Weight = FontWeight::Bold;
  titleLabel_->disableAutoTextPlacement = true;

  messageLabel_ = layoutGrid_->CreateLayoutChild<TextBlock>(
    GridLayoutOptions{ContentColumn, 2, 1, 1});
  messageLabel_->SetContent(options_.message);
  // messageLabel_->autoWidth = false;
  messageLabel_->autoHeight = false;
  // messageLabel_->disableAutoTextPlacement = true;

  if (hasDetails) {
    detailsButton_ = layoutGrid_->CreateLayoutChild<Button>(
      GridLayoutOptions{ContentColumn, 4, 1, 1});

    detailsButton_->SetContent(
      detailsVisible_ ? "Hide details" : "Show details");

    detailsButton_->OnClick = [this] {
      ToggleDetails();
    };

    detailsLabel_ = layoutGrid_->CreateLayoutChild<Label>(
      GridLayoutOptions{ContentColumn, 6, 1, 1});

    detailsLabel_->SetContent(options_.details);
    detailsLabel_->autoWidth = false;
    detailsLabel_->autoHeight = false;
    detailsLabel_->disableAutoTextPlacement = true;
    detailsLabel_->isVisible = detailsVisible_;
  }

  buttonPanel_ = layoutGrid_->CreateLayoutChild<GridLayoutPanel>(
    GridLayoutOptions{0, 9, ColumnCount, 1});

  buttonPanel_->SetGridRows({
    LayoutLength::Fill()
  });

  RebuildButtons();

  MarkLayoutDirty();
}

void MessageBox::RebuildButtons() {
  if (buttonPanel_ == nullptr) {
    return;
  }

  buttonPanel_->Children.clear();
  buttonPanel_->ownedChildren.clear();

  std::vector<LayoutLength> columns;
  columns.push_back(LayoutLength::Fill());

  for (size_t i = 0; i < options_.buttons.size(); ++i) {
    if (i > 0) {
      columns.push_back(LayoutLength::Fixed(ButtonSpacing));
    }

    columns.push_back(LayoutLength::Fixed(ButtonWidth));
  }

  buttonPanel_->SetGridColumns(columns);
  buttonPanel_->SetGridRows({
    LayoutLength::Fill()
  });

  int column = 1;

  for (size_t i = 0; i < options_.buttons.size(); ++i) {
    if (i > 0) {
      ++column; // skip spacing column
    }

    auto& buttonOptions = options_.buttons[i];

    auto* button = buttonPanel_->CreateLayoutChild<Button>(
      GridLayoutOptions{column, 0, 1, 1});

    button->SetContent(buttonOptions.text);

    button->OnClick = [this, result = buttonOptions.result] {
      Complete(result);
    };

    ++column;
  }

  MarkLayoutDirty();
}

void MessageBox::OnRender(Graphics* rendTarget) {
  (void)rendTarget;
}

void MessageBox::DoLayout(Graphics* renderTarget) {
  (void)renderTarget;

  const bool hasDetails = detailsLabel_ != nullptr;

  size = {
    std::min(SurfaceWidth, renderTarget->viewportSize.width - 50),
    std::min(GetMessageBoxHeight(hasDetails, detailsVisible_), renderTarget->viewportSize.height)
  };

  if (surface_ != nullptr) {
    surface_->SetLayoutFrame({0.0f, 0.0f}, size);
  }

  if (layoutGrid_ != nullptr) {
    layoutGrid_->SetLayoutFrame({0.0f, 0.0f}, size);
  }
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
    detailsButton_->SetContent(
      detailsVisible_ ? "Hide details" : "Show details");
  }

  if (detailsLabel_ != nullptr) {
    detailsLabel_->isVisible = detailsVisible_;
  }

  ConfigureMessageBoxRows(
    layoutGrid_,
    detailsLabel_ != nullptr,
    detailsVisible_);

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

std::optional<std::string> MessageBox::GetControlName() const {
  return "MessageBox";
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

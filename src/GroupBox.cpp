//
// Created by code on 4/21/26.
//

#include <kromakit/GroupBox.h>
#include <algorithm>
#include <kromakit/styling/controls/GroupBoxStyle.h>
#include <kromakit/styling/controls/PanelStyle.h>

GroupBox::GroupBox() {
  ContentPanel = CreateControl<Panel>();
  UseStyle(DUIDefaultStyles::GroupBoxStyles::Default);
  ContentPanel->UseStyle(DUIDefaultStyles::PanelStyles::Clear);
}

void GroupBox::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);

  (void)rendTarget->RenderText(
    sContent.c_str(), font, foreground,
    Spacing.x, textY_);
}

void GroupBox::DoLayout(Graphics *renderTarget) {
  auto [tWidth, tHeight] = renderTarget->CalculateTextSize(
    sContent.c_str(), font);
  (void)tWidth;

  textY_ = Spacing.y;

  ContentPanel->location.x = Spacing.x;
  ContentPanel->location.y = textY_ + tHeight + Spacing.y;

  ContentPanel->size = {
    std::max(0.0f, size.width - ContentPanel->location.x - Spacing.width),
    std::max(0.0f, size.height - ContentPanel->location.y - Spacing.height)
  };

  if (ContentPanel != nullptr) {
    for (Control* child : ContentPanel->Children) {
      if (child == nullptr)
        continue;

      child->SetLayoutLocation({0.0f, 0.0f});
      child->SetLayoutSize(ContentPanel->size);
    }

    ContentPanel->contentLayoutSize = ContentPanel->size;
  }
}

// void GroupBox::SetClientHeight(float h) {
//
// }

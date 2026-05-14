//
// Created by code on 4/26/26.
//

#include <kromakit/TextBlock.h>
#include <kromakit/styling/controls/TextBlockStyle.h>

TextBlock::TextBlock() {
  UseStyle(DUIDefaultStyles::TextBlockStyles::Default);
}

void TextBlock::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);

  if (!cachedTextSizeDirty) {
    rendTarget->RenderTextBox(
      sContent.c_str(), font,
      foreground, 0, 0, size.width);
  }
}

void TextBlock::DoLayout(Graphics *renderTarget) {
  cachedTextSize = renderTarget->CalculateTextBoxSize(
    sContent.c_str(), size.width, font);

  if (autoHeight) {
    size.height = cachedTextSize.height;
  }

  cachedTextSizeDirty = false;
}

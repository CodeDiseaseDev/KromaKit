//
// Created by code on 5/9/26.
//

#include <kromakit/text_input/TextBox.h>

#include <kromakit/TextInputBridge.h>
#include <kromakit/platform/Platform.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

#include <algorithm>
#include <cmath>

#include <kromakit/context_menu/ContextMenu.h>
#include <kromakit/context_menu/item_types/ContextMenuSeparator.h>
#include <kromakit/context_menu/item_types/ContextMenuTextItem.h>
#include <kromakit/platform/DUIClipboard.h>


bool TextBox::HasSelection() const {
  return selectionAnchorIndex != selectionFocusIndex;
}

int TextBox::GetSelectionStart() const {
  return std::min(selectionAnchorIndex, selectionFocusIndex);
}

int TextBox::GetSelectionEnd() const {
  return std::max(selectionAnchorIndex, selectionFocusIndex);
}

void TextBox::ClearSelection() {
  selectionAnchorIndex = cursorIndex;
  selectionFocusIndex = cursorIndex;
}

void TextBox::SelectAll() {
  if (!sContent.empty()) {
    SetSelection(0, static_cast<int>(sContent.size()));
  }
}

void TextBox::SetSelection(
  int anchor,
  int focus
) {
  const int textLength = static_cast<int>(sContent.size());

  selectionAnchorIndex = std::clamp(anchor, 0, textLength);
  selectionFocusIndex = std::clamp(focus, 0, textLength);

  cursorIndex = selectionFocusIndex;
  UpdateCursor();
}

void TextBox::SetPlaceholderText(std::string text) {
  placeholderText = text;
}

std::string TextBox::GetPlaceholderText() {
  return placeholderText;
}

void TextBox::MarkTextMetricsDirty() {
  caretPositionsDirty = true;
}

void TextBox::RebuildCaretPositions(Graphics* graphics) const {
  if (!caretPositionsDirty)
    return;

  caretXPositions.clear();
  caretXPositions.reserve(sContent.size() + 1);

  caretXPositions.push_back(0.0f);

  for (int i = 1; i <= static_cast<int>(sContent.size()); i++) {
    std::string beforeIndex = sContent.substr(
      0,
      static_cast<size_t>(i));

    caretXPositions.push_back(
      graphics->CalculateTextAdvance(
        beforeIndex.c_str(),
        font));
  }

  caretPositionsDirty = false;
}

void TextBox::RebuildCaretPositions(std::shared_ptr<Graphics> graphics) const {
  RebuildCaretPositions(graphics.get());
}

float TextBox::CalculateTextWidthUpToIndex(
  int       index,
  Graphics* rendTarget
) {
  RebuildCaretPositions(rendTarget);

  index = std::clamp(
    index,
    0,
    static_cast<int>(sContent.size()));

  return caretXPositions[static_cast<size_t>(index)];
}

// float TextBox::CalculateTextWidthUpToIndex(
//   int       index,
//   Graphics* rendTarget
// ) {
//   RebuildCaretPositions(rendTarget);
// }

int TextBox::GetTextIndexAtLocalX(float localX) {
  std::shared_ptr<Graphics> graphics = GetRootWindow()->graphics;

  RebuildCaretPositions(graphics);

  const float targetX = localX - xSideSpacing;

  if (targetX <= 0.0f)
    return 0;

  const int textLength = static_cast<int>(sContent.size());

  if (textLength <= 0)
    return 0;

  if (targetX >= caretXPositions.back())
    return textLength;

  auto it = std::lower_bound(
    caretXPositions.begin(),
    caretXPositions.end(),
    targetX);

  int rightIndex = static_cast<int>(
    std::distance(caretXPositions.begin(), it));

  rightIndex = std::clamp(
    rightIndex,
    0,
    textLength);

  int leftIndex = std::max(
    0,
    rightIndex - 1);

  const float leftDistance = std::abs(
    targetX - caretXPositions[static_cast<size_t>(leftIndex)]);

  const float rightDistance = std::abs(
    caretXPositions[static_cast<size_t>(rightIndex)] - targetX);

  return rightDistance < leftDistance
    ? rightIndex
    : leftIndex;
}

void TextBox::MoveCursorToLocalX(float localX) {
  cursorIndex = GetTextIndexAtLocalX(localX);
  ClearSelection();
  UpdateCursor();
}

void TextBox::MoveCursorWithSelection(
  int  nextIndex,
  bool extendSelection
) {
  nextIndex = std::clamp(
    nextIndex,
    0,
    static_cast<int>(sContent.size()));

  if (extendSelection) {
    if (!HasSelection())
      selectionAnchorIndex = cursorIndex;

    selectionFocusIndex = nextIndex;
    cursorIndex = nextIndex;
  } else {
    cursorIndex = nextIndex;
    ClearSelection();
  }

  UpdateCursor();
}

void TextBox::UpdateCursor() {
  if (cachedGraphics_ != nullptr) {
    cursorXPos =
      xSideSpacing +
      CalculateTextWidthUpToIndex(
        cursorIndex,
        cachedGraphics_);

    MarkVisualDirty();
  }

}

void TextBox::OnPointerDown(const PointerEvent& e) {
  if (e.mouseButton != MouseButton::Left)
    return;

  const int index = GetTextIndexAtLocalX(e.position.x);

  cursorIndex = index;
  selectionAnchorIndex = index;
  selectionFocusIndex = index;
  isSelecting = true;

  UpdateCursor();

  DirectUITextInput::ShowKeyboard();
}

void TextBox::OnPointerMove(const PointerEvent& e) {
  if (!isSelecting)
    return;

  const int index = GetTextIndexAtLocalX(e.position.x);

  selectionFocusIndex = index;
  cursorIndex = index;

  UpdateCursor();
}

void TextBox::OnPointerUp(const PointerEvent& e) {
  if (e.mouseButton == MouseButton::Left)
    isSelecting = false;
}

void TextBox::OnTextInput(const DUITextInputEvent& textInputEvent) {
  if (textInputEvent.codepoint == 0)
    return;

  AddChar(textInputEvent.codepoint);

  if (OnInput != nullptr)
    OnInput();
}

void TextBox::OnKeyDown(const DUIKeyEvent& keyEvent) {
  switch (keyEvent.key) {
    case DUIKey::Left:
      MoveCursorWithSelection(
        cursorIndex - 1,
        keyEvent.shift);
      break;

    case DUIKey::Right:
      MoveCursorWithSelection(
        cursorIndex + 1,
        keyEvent.shift);
      break;

    case DUIKey::Backspace:
      if (HasSelection()) {
        DeleteSelection();

        if (OnInput != nullptr)
          OnInput();

        break;
      }

      if (cursorIndex > 0) {
        DeleteCharAt(static_cast<size_t>(cursorIndex - 1));
        cursorIndex--;

        ClearSelection();
        UpdateCursor();

        if (OnInput != nullptr)
          OnInput();
      }
      break;

    case DUIKey::Delete:
      if (HasSelection()) {
        DeleteSelection();

        if (OnInput != nullptr)
          OnInput();

        break;
      }

      if (cursorIndex < static_cast<int>(sContent.size())) {
        DeleteCharAt(static_cast<size_t>(cursorIndex));

        ClearSelection();
        UpdateCursor();

        if (OnInput != nullptr)
          OnInput();
      }
      break;

    default:
      break;
  }
}

void TextBox::OnKeyUp(const DUIKeyEvent& keyEvent) {
  Control::OnKeyUp(keyEvent);
}

void TextBox::DeleteSelection() {
  if (!HasSelection())
    return;

  const int start = GetSelectionStart();
  const int end = GetSelectionEnd();

  sContent.erase(
    static_cast<size_t>(start),
    static_cast<size_t>(end - start));

  cursorIndex = start;
  ClearSelection();

  MarkTextMetricsDirty();
  UpdateCursor();
}

void TextBox::DeleteCharAt(size_t index) {
  if (index >= sContent.size())
    return;

  sContent.erase(index, 1);

  MarkTextMetricsDirty();
}

void TextBox::AddChar(char32_t c) {
  if (HasSelection())
    DeleteSelection();

  sContent.insert(
    static_cast<size_t>(cursorIndex),
    1,
    static_cast<char>(c));

  cursorIndex++;

  ClearSelection();
  MarkTextMetricsDirty();
  UpdateCursor();
}

std::string TextBox::GetSelectedText() const {
  if (!HasSelection())
    return "";

  const int start = GetSelectionStart();
  const int end = GetSelectionEnd();

  return sContent.substr(
    static_cast<size_t>(start),
    static_cast<size_t>(end - start));
}

void TextBox::CopySelection() {
  if (!HasSelection())
    return;

  DUIClipboard::SetText(GetSelectedText());
}

void TextBox::CutSelection() {
  if (!HasSelection())
    return;

  DUIClipboard::SetText(GetSelectedText());
  DeleteSelection();

  if (OnInput != nullptr)
    OnInput();
}

void TextBox::PasteFromClipboard() {
  std::string text = DUIClipboard::GetText();

  if (text.empty())
    return;

  if (HasSelection())
    DeleteSelection();

  sContent.insert(
    static_cast<size_t>(cursorIndex),
    text);

  cursorIndex += static_cast<int>(text.size());

  ClearSelection();
  MarkTextMetricsDirty();
  UpdateCursor();

  if (OnInput != nullptr)
    OnInput();
}

void TextBox::RenderSelection(Graphics* rendTarget) {
  if (!HasSelection())
    return;

  const int start = GetSelectionStart();
  const int end = GetSelectionEnd();

  const float startX =
    xSideSpacing +
    CalculateTextWidthUpToIndex(
      start,
      rendTarget);

  const float endX =
    xSideSpacing +
    CalculateTextWidthUpToIndex(
      end,
      rendTarget);

  const float selectionHeight = font.FontSize + 5;

  const static Color selected = ColorFromRGB(80, 120, 255, 120);
  const static Color unfocused = ColorFromRGB(120, 120, 120, 120);

  rendTarget->FillRoundedRectangle(
    startX,
    size.height / 2 - selectionHeight / 2,
    endX - startX,
    selectionHeight,
    isSelected ? selected : unfocused, 3);
}

void TextBox::RenderTextBoxText(Graphics* rendTarget) {
  const bool shouldDisplayPlaceholder = sContent.empty();

  const char* str =
    shouldDisplayPlaceholder
      ? placeholderText.c_str()
      : sContent.c_str();

  auto textSize = rendTarget->CalculateTextSize(
    str,
    font);

  auto fg = foreground;

  if (shouldDisplayPlaceholder) {
    fg = rendTarget->ColorBrightnessOffset(
      fg,
      -0.6);
  }

  rendTarget->RenderText(
    str,
    font,
    fg,
    xSideSpacing,
    size.height / 2.0f - textSize.height / 2.0f);
}

void TextBox::OnRender(Graphics* rendTarget) {
  cachedGraphics_ = rendTarget;

  rendTarget->FillControlBackground(this);

  RenderSelection(rendTarget);
  RenderTextBoxText(rendTarget);

  if (!isSelected)
    return;

  if (HasSelection())
    return;

  rendTarget->FillRectangle(
    cursorXPos,
    size.height / 2.0f - cursorHeight / 2.0f,
    1.0f,
    cursorHeight,
    Alpha(foreground, blinkAnimValue));
}

void TextBox::DoLayout(Graphics* renderTarget) {

}

void TextBox::OnIsSelectedChanged() {
  blinkAnimationTime = 0.0f;

  if (Platform::Instance.os != Platform::OS::iOS)
    return;

  if (!isSelected)
    DirectUITextInput::HideKeyboard();
}

bool TextBox::WantsClickCapture() const {
  return true;
}

bool TextBox::WantsDragCapture() const {
  return true;
}

bool TextBox::HandleShortcut(const DUIKeyEvent& keyEvent) {
  const bool command = keyEvent.ctrl || keyEvent.super;

  if (!command)
    return false;

  switch (keyEvent.key) {
    case DUIKey::A:
      SetSelection(0, static_cast<int>(sContent.size()));
      return true;

    case DUIKey::C:
      CopySelection();
      return true;

    case DUIKey::X:
      CutSelection();
      return true;

    case DUIKey::V:
      PasteFromClipboard();
      return true;

    default:
      return false;
  }
}

TextBox::TextBox() {
  UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Panel);
  UseStyle(DUIDefaultStyles::ForegroundBaseStyles::Primary);

  SetContextMenuProvider([this] {
    auto menu = std::make_unique<ContextMenu>();

    if (HasSelection()) {
      menu->AddItem<ContextMenuTextItem>("Cut")
        ->OnClick = [this] {
          // CutSelection();
        };
      menu->AddItem<ContextMenuSeparator>();

      menu->AddItem<ContextMenuTextItem>("Copy")
        ->OnClick = [this] {
          // CopySelection();
        };
      menu->AddItem<ContextMenuSeparator>();

      menu->AddItem<ContextMenuTextItem>("Delete")
        ->OnClick = [this] {
          DeleteSelection();

          if (OnInput != nullptr)
            OnInput();
        };
      menu->AddItem<ContextMenuSeparator>();

      menu->AddItem<ContextMenuTextItem>("Clear Selection")
        ->OnClick = [this] {
          ClearSelection();
        };

      menu->AddItem<ContextMenuSeparator>();
    }

    menu->AddItem<ContextMenuTextItem>("Paste")
      ->OnClick = [this] {
        // PasteFromClipboard();
      };
    menu->AddItem<ContextMenuSeparator>();

    menu->AddItem<ContextMenuTextItem>("Select All")
      ->OnClick = [this] {
        SetSelection(0, static_cast<int>(sContent.size()));
      };
    menu->AddItem<ContextMenuSeparator>();

    menu->AddItem<ContextMenuTextItem>("Clear")
      ->OnClick = [this] {
        sContent.clear();
        cursorIndex = 0;
        ClearSelection();
        MarkTextMetricsDirty();
        UpdateCursor();

        if (OnInput != nullptr)
          OnInput();
      };
    menu->AddItem<ContextMenuSeparator>();

    return menu;
  });
}

void TextBox::Update(float deltaTime) {
  UpdateBlinkAnimation(deltaTime);
}

float SmoothStep(float x) {
  x = std::clamp(x, 0.0f, 1.0f);
  return x * x * (3.0f - 2.0f * x);
}

float SmootherStep(float x) {
  x = std::clamp(x, 0.0f, 1.0f);
  return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

void TextBox::UpdateBlinkAnimation(float deltaTime) {
  constexpr float cyclesPerSecond = 1.25f;

  blinkAnimationTime += deltaTime * cyclesPerSecond;

  if (blinkAnimationTime >= 1.0f)
    blinkAnimationTime -= 1.0f;

  const float phase = blinkAnimationTime;

  const float pingPong =
    phase < 0.5f
      ? phase * 2.0f
      : (1.0f - phase) * 2.0f;

  blinkAnimValue = SmootherStep(pingPong);
}
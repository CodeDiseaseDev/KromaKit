//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_TEXTBOX_H
#define TEST_IOS_APP_TEXTBOX_H

#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>

class TextBox : public Control {
  int cursorIndex = 0;

  int selectionAnchorIndex = 0;
  int selectionFocusIndex = 0;

  bool isSelecting = false;

  float blinkAnimationTime = 0.0f;
  float blinkAnimValue = 0.0f;


public:
  std::function<void()> OnInput;


  // int cursorIndex = 0;

  TextBox();

  void Update(float deltaTime) override;
  void UpdateBlinkAnimation(float deltaTime);

  void OnRender(Graphics* rendTarget) override;

  void DeleteSelection();

  void DoLayout(Graphics* renderTarget) override;

  void OnPointerDown(const PointerEvent& e) override;
  void OnPointerMove(const PointerEvent& e) override;
  void OnPointerUp(const PointerEvent &e) override;

  void CopySelection();

  void CutSelection();

  void PasteFromClipboard();

  void RenderSelection(Graphics *rendTarget);
  void OnKeyDown(const DUIKeyEvent& keyEvent) override;
  void MoveCursorWithSelection(int nextIndex, bool extendSelection);
  void OnKeyUp(const DUIKeyEvent& keyEvent) override;
  void OnTextInput(const DUITextInputEvent& textInputEvent) override;
  void OnIsSelectedChanged() override;
  bool HandleShortcut(const DUIKeyEvent &keyEvent) override;

  std::optional<std::string> GetControlName() const override { return "TextBox"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  bool WantsClickCapture() const override;
  bool WantsDragCapture() const override;

  float CalculateTextWidthUpToIndex(int index, Graphics *rendTarget);

private:
  static constexpr float xSideSpacing = 10.0f;
  static constexpr float cursorHeight = 15.0f;

  float cursorXPos = xSideSpacing;

  std::string placeholderText = "TextBox";

  mutable bool caretPositionsDirty = true;
  mutable std::vector<float> caretXPositions;

  void MarkTextMetricsDirty();
  void RebuildCaretPositions(Graphics* graphics) const;

  bool HasSelection() const;

  int GetSelectionStart() const;

  int GetSelectionEnd() const;

  void ClearSelection();

  void SetSelection(int anchor, int focus);

  int GetTextIndexAtLocalX(float localX);

  void MoveCursorToLocalX(float localX);

  // float CalculateCharWidthAt(
  //   int       index,
  //   Graphics* rendTarget
  // );
  //
  //
  //
  // void UpdateCursorAfterClick(DUIPoint cursorLocation);
  void UpdateCursor();

  void DeleteCharAt(size_t index);
  void AddChar(char32_t c);

  std::string GetSelectedText() const;

  void RenderTextBoxText(Graphics* rendTarget);
};



#endif //TEST_IOS_APP_TEXTBOX_H

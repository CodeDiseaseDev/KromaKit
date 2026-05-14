//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_KEYBOARDAWAREPANEL_H
#define TEST_IOS_APP_KEYBOARDAWAREPANEL_H

#include <kromakit/Panel.h>

class KeyboardAwarePanel : public Control {
public:
  KeyboardAwarePanel();

  float CalculateKeyboardOverlap() const;

  void SetKeyboardFrame(DUIRect keyboardFrameInWindow);
  void SetKeyboardVisible(bool visible);

  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics* renderTarget) override;
  void Update(float deltaTime) override;

  std::optional<std::string> GetControlName() const override { return "KeyboardAwarePanel"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  void SetBorrowedChild(Control* ctrl) {
    if (borrowedChild_ == ctrl)
      return;

    if (borrowedChild_ != nullptr)
      RemoveControl(borrowedChild_);

    borrowedChild_ = nullptr;

    if (ctrl != nullptr) {
      borrowedChild_ = ctrl;
      AddBorrowedControl(ctrl);
    }
  }

private:
  bool keyboardVisible = false;

  DUIRect keyboardFrame = {};
  float keyboardInset = 0.0f;
  float animatedInset = 0.0f;

  float animationSpeed = 18.0f;

  // Panel* panel_ = nullptr;
  Control* borrowedChild_ = nullptr;
};



#endif //TEST_IOS_APP_KEYBOARDAWAREPANEL_H

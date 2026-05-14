//
// Created by code on 4/27/26.
//

#ifndef TEST_IOS_APP_ICONTEXTMENUITEM_H
#define TEST_IOS_APP_ICONTEXTMENUITEM_H

#include <kromakit/graphics/Graphics.h>
#include <vector>

#include <kromakit/Control.h>
#include <kromakit/Label.h>
#include <kromakit/animation/FloatAnimation.h>

class IContextMenuItem :
  public virtual Control {

  FloatAnimation tapAnimation = FloatAnimation(
    1, 0, 0.2, AnimationCurve::EaseOut);


public:
  IContextMenuItem();

  Color tapColor;

  std::function<void()> OnClick = nullptr;
  std::function<void()> RequestDismiss = nullptr;


  Label* label = nullptr;

  void SetLabelText(std::string str);

  void DoLayout(Graphics *renderTarget) override;
  void OnRender(Graphics *rendTarget) override = 0;

  void Update(float deltaTime) override;

  std::optional<std::string> GetControlName() const override { return "IContextMenuItem"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  void OnPointerDown(const PointerEvent &e) override;
  void OnPointerUp(const PointerEvent &e) override;
  void OnPointerCancel(const PointerEvent &e) override;

  bool WantsClickCapture() const override { return true; }

  float LabelXSpacing = 17;
};



#endif // TEST_IOS_APP_ICONTEXTMENUITEM_H

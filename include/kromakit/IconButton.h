//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_ICONBUTTON_H
#define TEST_IOS_APP_ICONBUTTON_H
#include <kromakit/Button.h>
#include <kromakit/svg/SvgIcon.h>


class IconButton :
  public Button {

  SvgIcon* icon_ = nullptr;
  float radius = 10.0f;

public:
  IconButton(std::string_view svg_path);

  void SetRadius(float value);

  std::optional<std::string> GetControlName() const override { return "IconButton"; }
  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics *renderTarget) override;

  void SetIcon(std::string_view svg_path);

  void SetTint(Color color);
  void ClearTint();
};



#endif //TEST_IOS_APP_ICONBUTTON_H

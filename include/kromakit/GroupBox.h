//
// Created by code on 4/21/26.
//

#ifndef TEST_IOS_APP_GROUPBOX_H
#define TEST_IOS_APP_GROUPBOX_H
#include <kromakit/Control.h>
#include <kromakit/Panel.h>


class GroupBox :
  public Control {

  float textY_ = 0.0f;

public:
  Panel* ContentPanel = nullptr;
  DUIRect Spacing = DUIRect(10);

  GroupBox();

  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics *renderTarget) override;

  std::optional<std::string> GetControlName() const override { return "GroupBox"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  // void SetClientHeight(float h);

};




#endif //TEST_IOS_APP_GROUPBOX_H

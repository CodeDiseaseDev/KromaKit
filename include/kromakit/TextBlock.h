//
// Created by code on 4/26/26.
//

#ifndef TEST_IOS_APP_TEXTBLOCK_H
#define TEST_IOS_APP_TEXTBLOCK_H
#include <kromakit/Control.h>


class TextBlock :
  public Control {
public:
  TextBlock();

  bool autoHeight = true;

  bool cachedTextSizeDirty = true;
  DUISize cachedTextSize {0,0};

  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics *renderTarget) override;

  std::optional<std::string> GetControlName() const override { return "TextBlock"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }
};



#endif //TEST_IOS_APP_TEXTBLOCK_H

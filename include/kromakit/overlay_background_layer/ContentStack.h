//
// Created by code on 5/3/26.
//

#ifndef TEST_IOS_APP_CONTENTSTACK_H
#define TEST_IOS_APP_CONTENTSTACK_H
#include <kromakit/Panel.h>


class ContentStack : public Panel {
public:
  ContentStack();

  void SetZoomScale(float scale);
  float GetZoomScale() const;
  void SetOverlayDepthEffect(float t);

  void OnRender(Graphics *rendTarget) override;
  bool ShouldClipToBounds() override;

  std::optional<std::string> GetControlName() const override;

private:
  float zoomScale = 1.0f;
  float overlayDepthEffect = 0.0f;

  float GetActualScale() const;
};



#endif //TEST_IOS_APP_CONTENTSTACK_H

//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGICON_H
#define TEST_IOS_APP_SVGICON_H
#include <kromakit/Control.h>
#include <optional>

class SvgIcon :
  public Control {

  std::shared_ptr<SvgDocument> svg_icon_ = nullptr;
  std::optional<Color> tint_ = std::nullopt;

public:
  SvgIcon();
  SvgIcon(std::string_view svg_path);

  void SetTint(Color color);
  void ClearTint();

  std::optional<std::string> GetControlName() const override { return "SvgIcon"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  void OnRender(Graphics* rendTarget) override;
  void DoLayout(Graphics* rendTarget) override;

  void LoadSVG(std::string_view svg_path);
};



#endif //TEST_IOS_APP_SVGICON_H

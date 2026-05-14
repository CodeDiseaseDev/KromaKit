#pragma once
#include <kromakit/Control.h>
#include <cmath>

class ControlPreview :
    public Control
{
  Color danger_lines_color =
    ColorFromRGB(255, 255, 0, 80);

public:
  Control* TargetControl = nullptr;

  void OnRender(Graphics* graphics) override;
  void DoLayout(Graphics* renderTarget) override;
};


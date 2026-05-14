//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_STACKLAYOUT_H
#define TEST_IOS_APP_STACKLAYOUT_H

#include <kromakit/layout/ILayout.h>

class IStackLayout : public ILayout {
public:
  enum class Direction {
    Horizontal,
    Vertical
  };


private:
  Direction direction = Direction::Vertical;
  float spacing = 0.0f;
  DUIInsets layoutPadding = {};

public:
  explicit IStackLayout(Direction direction);

  virtual DUISize Measure(const DUISize& available) = 0;

  void SetSpacing(float value);
  void SetPadding(const DUIInsets& value);

  DUISize Measure(
      const std::vector<LayoutItem>& children,
      const DUISize& available
  ) override;

  void Arrange(
      std::vector<LayoutItem>& children,
      const DUIRect& rect
  ) override;
};



#endif //TEST_IOS_APP_STACKLAYOUT_H

//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_IHSTACKLAYOUT_H
#define TEST_IOS_APP_IHSTACKLAYOUT_H

#include <kromakit/layout/layout_types/IStackLayout.h>

class IHStackLayout :
  protected IStackLayout {
public:
  IHStackLayout()
    : IStackLayout(Direction::Horizontal) {
  }

  DUISize Measure(const DUISize& available) override = 0;

  ~IHStackLayout() override = default;
};



#endif //TEST_IOS_APP_IHSTACKLAYOUT_H

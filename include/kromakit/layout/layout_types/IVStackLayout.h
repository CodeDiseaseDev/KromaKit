//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_VSTACKLAYOUT_H
#define TEST_IOS_APP_VSTACKLAYOUT_H



#include <kromakit/layout/layout_types/IStackLayout.h>

class IVStackLayout :
  protected IStackLayout {
public:

  IVStackLayout()
    : IStackLayout(Direction::Vertical) {
  }

  DUISize Measure(const DUISize& available) override = 0;

  ~IVStackLayout() override = default;
};


#endif //TEST_IOS_APP_VSTACKLAYOUT_H

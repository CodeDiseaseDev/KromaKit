//
// Created by code on 4/26/26.
//

#ifndef TEST_IOS_APP_ISTYLE_H
#define TEST_IOS_APP_ISTYLE_H

class Control;

class IStyle {
public:
  virtual ~IStyle() = default;
  virtual bool Apply(Control& control) const = 0;
};

#endif //TEST_IOS_APP_ISTYLE_H

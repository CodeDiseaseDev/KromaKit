//
// Created by code on 4/26/26.
//

#ifndef TEST_IOS_APP_DUISTYLE_H
#define TEST_IOS_APP_DUISTYLE_H


#include <kromakit/styling/IStyle.h>
#include <functional>
#include <kromakit/Utils.h>

template<typename TControl>
class DUIStyle : public IStyle {
public:
  using ApplyFunc = std::function<void(TControl&)>;

  explicit DUIStyle(ApplyFunc applyFunc)
    : applyFunc(std::move(applyFunc)) {
  }

  bool Apply(Control& control) const override {
    TControl* typedControl = dynamic_cast<TControl*>(&control);

    if (typedControl == nullptr) {
      Logging::Log("typedControl == nullptr\n");
      return false;
    }

    applyFunc(*typedControl);
    return true;
    // if constexpr (std::is_same_v<TControl, Control>) {
    //   applyFunc(control);
    //   return true;
    // } else {
    //   TControl* typedControl = dynamic_cast<TControl*>(&control);
    //
    //   if (typedControl == nullptr) {
    //     Logging::Log("typedControl == nullptr\n");
    //     return false;
    //   }
    //
    //   applyFunc(*typedControl);
    //   return true;
    // }
  }

private:
  ApplyFunc applyFunc;
};



#endif //TEST_IOS_APP_DUISTYLE_H

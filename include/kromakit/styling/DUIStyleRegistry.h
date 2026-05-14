//
// Created by code on 4/26/26.
//

#ifndef TEST_IOS_APP_DUISTYLEREGISTRY_H
#define TEST_IOS_APP_DUISTYLEREGISTRY_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

#include <kromakit/styling/DUIStyle.h>
#include <kromakit/styling/DUIStyleName.h>

class DUIStyleRegistry {
public:
  template<typename TControl, typename TApplyFunc>
  static void Register(const DUIStyleName& name, TApplyFunc&& applyFunc) {
    if (name.Empty()) {
      return;
    }

    auto& styles = GetStyles();

    if (styles.find(name) != styles.end()) {
      // You can swap this for your own logging/assert system
      printf("DUIStyleRegistry: style '%s' already registered\n", name.c_str());
      return; // prevent overwrite
    }

    styles[name] =
      std::make_unique<DUIStyle<TControl>>(
        typename DUIStyle<TControl>::ApplyFunc(
          std::forward<TApplyFunc>(applyFunc)
        )
      );

    Logging::Log("Style %s registered\n", name.c_str());
  }

  static bool Apply(const DUIStyleName& name, Control& control) {
    auto it = GetStyles().find(name);

    if (it == GetStyles().end()) {
      return false;
    }

    return it->second->Apply(control);
  }

  static bool Exists(const DUIStyleName& name) {
    return GetStyles().find(name) != GetStyles().end();
  }

private:
  static std::unordered_map<DUIStyleName, std::unique_ptr<IStyle>>& GetStyles() {
    static std::unordered_map<DUIStyleName, std::unique_ptr<IStyle>> styles;
    return styles;
  }
};



#endif //TEST_IOS_APP_DUISTYLEREGISTRY_H

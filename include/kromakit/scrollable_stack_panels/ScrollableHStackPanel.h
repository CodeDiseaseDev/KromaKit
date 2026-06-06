//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_SCROLLABLEHSTACKPANEL_H
#define TEST_IOS_APP_SCROLLABLEHSTACKPANEL_H

#include <algorithm>

#include <kromakit/stack_panels/HStackPanel.h>
#include <kromakit/gesture/scrolling/ScrollableView.h>

class ScrollableHStackPanel : public ScrollableView {
public:
  ScrollableHStackPanel();

  HStackPanel& Stack();
  const HStackPanel& Stack() const;


  // Children must be added to the inner stack so ScrollableView only owns viewport behaviour.
  // Use `.Stack().AddControl(...)`
  template<class T>
  T* AddControl(T*) = delete;
  template <typename T>
  T* AddControl(std::unique_ptr<T>) = delete;

  std::optional<std::string> GetControlName() const override {
    return "ScrollableHStackPanel";
  }

protected:
  void DoLayout(Graphics* renderTarget) override;
  void OnRender(Graphics *rendTarget) override;

private:
  HStackPanel stackPanel;
};


#endif // TEST_IOS_APP_SCROLLABLEHSTACKPANEL_H

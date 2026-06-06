//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_SCROLLABLEVSTACKPANEL_H
#define TEST_IOS_APP_SCROLLABLEVSTACKPANEL_H

#include <kromakit/stack_panels/VStackPanel.h>

#include <algorithm>

#include <kromakit/stack_panels/VStackPanel.h>
#include <kromakit/gesture/scrolling/ScrollableView.h>

class ScrollableVStackPanel : public ScrollableView {
public:
  ScrollableVStackPanel();

  VStackPanel& Stack();
  const VStackPanel& Stack() const;


  // Children must be added to the inner stack so ScrollableView only owns viewport behaviour.
  // Use `.Stack().AddControl(...)`
  template<class T>
  T* AddControl(T*) = delete;
  template <typename T>
  T* AddControl(std::unique_ptr<T>) = delete;

  std::optional<std::string> GetControlName() const override {
    return "ScrollableVStackPanel";
  }

protected:
  void DoLayout(Graphics* renderTarget) override;
  void OnRender(Graphics *rendTarget) override;
  void OnPointerMove(const PointerEvent& e) override;
  void OnScroll(const PointerEvent& e) override;

private:
  VStackPanel stackPanel;
};


#endif // TEST_IOS_APP_SCROLLABLEVSTACKPANEL_H

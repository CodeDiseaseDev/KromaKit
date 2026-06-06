//
// Created by code on 5/1/26.
//

#ifndef TEST_IOS_APP_FREESCROLLVSTACKPANEL_H
#define TEST_IOS_APP_FREESCROLLVSTACKPANEL_H

#include <algorithm>

#include <kromakit/stack_panels/VStackPanel.h>
#include <kromakit/gesture/scrolling/ScrollableView.h>

class FreeScrollVStackPanel : public ScrollableView {
public:
  FreeScrollVStackPanel();

  VStackPanel& Stack();
  const VStackPanel& Stack() const;


  // Children must be added to the inner stack so ScrollableView only owns viewport behaviour.
  // Use `.Stack().AddControl(...)`
  template<class T>
  T* AddControl(T*) = delete;
  template <typename T>
  T* AddControl(std::unique_ptr<T>) = delete;

  std::optional<std::string> GetControlName() const override {
    return "FreeScrollVStackPanel";
  }


protected:
  void DoLayout(Graphics* renderTarget) override;
  void OnRender(Graphics *rendTarget) override;
  void OnPointerMove(const PointerEvent& e) override;
  void OnScroll(const PointerEvent& e) override;

private:
  VStackPanel stackPanel;
};


#endif // TEST_IOS_APP_FREESCROLLVSTACKPANEL_H

// #pragma once
// #include <kromakit/Control.h>
// #include <kromakit/Label.h>
// #include <kromakit/Button.h>
//
// #include <functional>
//
// class MessageBoxCtrl :
//     public Control
// {
// public:
//   enum MsgBoxResult {
//     MSGBOX_FALSE,
//     MSGBOX_TRUE
//   };
//
//   float Padding = 10;
//
//   Label* title = nullptr;
//   Label* content = nullptr;
//   Button* okBtn = nullptr;
//   Button* cancelBtn = nullptr;
//
//   std::function<void(MsgBoxResult)> OnResult;
//
//   MessageBoxCtrl();
//
//   void Show(
//     const wchar_t* content,
//     const wchar_t* title
//   );
//
//   void OnRender(Graphics* rendTarget) override;
//   void DoLayout(Graphics* rendTarget) override;
//
//   void OnUsed() override;
// };
//

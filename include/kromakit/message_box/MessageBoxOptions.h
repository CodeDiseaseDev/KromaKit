#pragma once

#include <functional>
#include <string>
#include <vector>


class Panel;
class Label;
class Button;
class OverlayBackgroundLayer;

enum class MessageBoxKind {
  Info,
  Success,
  Warning,
  Error,
  Question
};

enum class MessageBoxResult {
  None,
  Ok,
  Cancel,
  Yes,
  No,
  Retry,
  Ignore,
  Custom
};

struct MessageBoxButton {
  std::string text;
  MessageBoxResult result = MessageBoxResult::Custom;

  bool isDefault = false;
  bool isCancel = false;
  bool destructive = false;

  std::function<void()> onClick;
};

struct MessageBoxOptions {
  MessageBoxKind kind = MessageBoxKind::Info;

  std::string title;
  std::string message;
  std::string details;

  bool showDetailsByDefault = false;
  bool dismissOnBackgroundTap = false;
  bool allowCopyMessage = true;
  bool allowCopyDetails = true;

  std::vector<MessageBoxButton> buttons;

  std::function<void(MessageBoxResult)> onResult;
};
#pragma once

enum class PresentationStyle {
  Auto,
  Modal,
  FloatingWindow
};

enum class PresentationDismissReason {
  Programmatic,
  BackgroundTap,
  CloseButton,
  HostStyleSwitch
};

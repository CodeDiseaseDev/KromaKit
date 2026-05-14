#pragma once

namespace DirectUIHaptics {

using ImpactFn = void(*)(int);
using SelectionFn = void(*)();
using NotifyFn = void(*)(int);

inline ImpactFn gImpact = nullptr;
inline SelectionFn gSelection = nullptr;
inline NotifyFn gNotify = nullptr;

inline void Register(
  ImpactFn impact,
  SelectionFn selection,
  NotifyFn notify)
{
  gImpact = impact;
  gSelection = selection;
  gNotify = notify;
}

inline void Impact(int style) {
  if (gImpact != nullptr) {
    gImpact(style);
  }
}

inline void Selection() {
  if (gSelection != nullptr) {
    gSelection();
  }
}

inline void Notify(int type) {
  if (gNotify != nullptr) {
    gNotify(type);
  }
}

} // namespace DirectUIHaptics

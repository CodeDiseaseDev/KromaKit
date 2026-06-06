//
// Created by code on 6/3/26.
//

#ifndef SYNTHEM_DUIWINDOWCLOSEEVENT_H
#define SYNTHEM_DUIWINDOWCLOSEEVENT_H

enum class DUIWindowCloseReason {
  NativeWindowClose,
  AppTerminate
};

struct DUIWindowCloseEvent {
  DUIWindowCloseReason reason = DUIWindowCloseReason::NativeWindowClose;
  bool cancel = false;
};

#endif //SYNTHEM_DUIWINDOWCLOSEEVENT_H

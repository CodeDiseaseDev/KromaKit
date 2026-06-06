//
// Created by code on 6/2/26.
//

#ifndef KROMAKIT_LINUXMAINTHREADDISPATCHER_H
#define KROMAKIT_LINUXMAINTHREADDISPATCHER_H

#if defined(__linux__) && !defined(__ANDROID__)

#include <kromakit/platform/QueuedMainThreadDispatcher.h>

class LinuxMainThreadDispatcher final :
  public QueuedMainThreadDispatcher {
public:
  LinuxMainThreadDispatcher() = default;
};

#endif

#endif // KROMAKIT_LINUXMAINTHREADDISPATCHER_H

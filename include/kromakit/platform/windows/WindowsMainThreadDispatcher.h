//
// Created by code on 6/2/26.
//

#ifndef KROMAKIT_WINDOWSMAINTHREADDISPATCHER_H
#define KROMAKIT_WINDOWSMAINTHREADDISPATCHER_H

#ifdef _WIN32

#include <kromakit/platform/QueuedMainThreadDispatcher.h>

class WindowsMainThreadDispatcher final :
  public QueuedMainThreadDispatcher {
public:
  WindowsMainThreadDispatcher() = default;
};

#endif

#endif // KROMAKIT_WINDOWSMAINTHREADDISPATCHER_H

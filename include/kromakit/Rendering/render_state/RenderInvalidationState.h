//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_RENDERINVALIDATIONSTATE_H
#define TEST_IOS_APP_RENDERINVALIDATIONSTATE_H
#include <cstdint>

struct RenderInvalidationState {
  uint64_t layoutVersion = 1;
  uint64_t renderTreeVersion = 1;
  uint64_t renderOrderVersion = 1;
  uint64_t visualVersion = 1;

  bool layoutDirty = false;
  bool renderTreeDirty = false;
  bool visualDirty = false;

  bool operator==(const RenderInvalidationState&) const = default;
};

#endif //TEST_IOS_APP_RENDERINVALIDATIONSTATE_H

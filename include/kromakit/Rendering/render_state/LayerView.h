//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_LAYERVIEW_H
#define TEST_IOS_APP_LAYERVIEW_H

#include <cstddef>
#include <vector>

struct LayerView {
  std::vector<size_t> indices;
  bool orderDirty = true;
  bool cullDirty = true;

  void Clear() {
    indices.clear();
    orderDirty = true;
    cullDirty = true;
  }

  void Reserve(size_t capacity) {
    if (indices.capacity() < capacity)
      indices.reserve(capacity);
  }
};

#endif //TEST_IOS_APP_LAYERVIEW_H

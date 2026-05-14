//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_RENDERCACHE_H
#define TEST_IOS_APP_RENDERCACHE_H
#include <cstddef>
#include <cstdint>
#include <vector>

#include <kromakit/Rendering/render_state/LayerView.h>
#include <kromakit/Rendering/render_state/RenderItem.h>


struct RenderOcclusionRegion {
  DUIRect rect;
  uint32_t treeStart = 0;
  uint32_t treeEnd = 0;
};

struct RenderCache {
  std::vector<RenderItem> items;
  LayerView content;
  LayerView overlay;
  std::vector<RenderOcclusionRegion> occlusionScratch;
  std::vector<size_t> overlayOccluderIndices;

  uint64_t treeVersion = 0;
  uint64_t layoutVersion = 0;
  uint64_t orderVersion = 0;
  uint64_t visualVersion = 0;
  DUISize viewportSize = {0, 0};
  float viewportPixelRatio = 0.0f;

  void ClearFrameLists() {
    items.clear();
    content.Clear();
    overlay.Clear();
  }

  void ReserveItems(size_t capacity) {
    if (items.capacity() < capacity)
      items.reserve(capacity);
  }

  void ReserveScratch(size_t capacity) {
    if (occlusionScratch.capacity() < capacity)
      occlusionScratch.reserve(capacity);
    if (overlayOccluderIndices.capacity() < capacity)
      overlayOccluderIndices.reserve(capacity);
  }
};



#endif //TEST_IOS_APP_RENDERCACHE_H

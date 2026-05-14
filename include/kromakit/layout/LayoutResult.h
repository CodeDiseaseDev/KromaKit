#ifndef TEST_IOS_APP_LAYOUTRESULT_H
#define TEST_IOS_APP_LAYOUTRESULT_H

struct LayoutResult {
  bool geometryChanged = false;
  bool needsAnotherPass = false;

  void Merge(const LayoutResult& other) {
    geometryChanged |= other.geometryChanged;
    needsAnotherPass |= other.needsAnotherPass;
  }
};

#endif // TEST_IOS_APP_LAYOUTRESULT_H

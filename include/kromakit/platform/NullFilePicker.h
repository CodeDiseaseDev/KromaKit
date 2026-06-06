//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_NULLFILEPICKER_H
#define SYNTHEM_NULLFILEPICKER_H
#include "IFilePicker.h"

class NullFilePicker  : public IFilePicker {
public:
  NullFilePicker() = default;
  ~NullFilePicker() override = default;

  void PickOpenFile(
    std::vector<std::string> allowedExtensions,
    std::function<void(std::optional<std::string>)> onResult
  ) override {

    if (onResult != nullptr) {
      onResult(std::nullopt);
    }

  }

  virtual void PickSaveFile(
    std::vector<std::string> allowedExtensions,
    std::string defaultFileName,
    std::function<void(std::optional<std::string>)> onResult
  ) {
    if (onResult != nullptr) {
      onResult(std::nullopt);
    }
  }
};

#endif //SYNTHEM_NULLFILEPICKER_H

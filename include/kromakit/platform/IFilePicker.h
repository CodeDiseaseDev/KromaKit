//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_IFILEPICKER_H
#define SYNTHEM_IFILEPICKER_H

#include <vector>
#include <functional>
#include <optional>
#include <string>


class IFilePicker {
public:
  virtual ~IFilePicker() = default;

  virtual void PickOpenFile(
    std::vector<std::string> allowedExtensions,
    std::function<void(std::optional<std::string>)> onResult
  ) = 0;

  virtual void PickSaveFile(
    std::vector<std::string> allowedExtensions,
    std::string defaultFileName,
    std::function<void(std::optional<std::string>)> onResult
  ) = 0;
};

#endif //SYNTHEM_IFILEPICKER_H

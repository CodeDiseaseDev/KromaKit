//
// Created by code on 6/1/26.
//

#ifndef SYNTHEM_WINDOWSFILEPICKER_H
#define SYNTHEM_WINDOWSFILEPICKER_H

#ifdef _WIN32

#include <kromakit/platform/IFilePicker.h>

#include <functional>
#include <optional>
#include <string>
#include <vector>

class WindowsFilePicker : public IFilePicker {
public:
  void PickOpenFile(
    std::vector<std::string> allowedExtensions,
    std::function<void(std::optional<std::string>)> onResult
  ) override;

  void PickSaveFile(
    std::vector<std::string> allowedExtensions,
    std::string defaultFileName,
    std::function<void(std::optional<std::string>)> onResult
  ) override;
};

#endif

#endif // SYNTHEM_WINDOWSFILEPICKER_H
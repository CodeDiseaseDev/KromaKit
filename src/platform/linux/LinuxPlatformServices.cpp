//
// Created by code on 5/30/26.
//

#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/linux/LinuxFilePicker.h>

#include <memory>

std::shared_ptr<IFilePicker> PlatformServices::FilePicker() {
  static std::shared_ptr<IFilePicker> picker =
    std::make_shared<LinuxFilePicker>();

  return picker;
}

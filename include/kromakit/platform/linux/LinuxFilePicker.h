//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_LINUXFILEPICKER_H
#define SYNTHEM_LINUXFILEPICKER_H
#include <kromakit/platform/IFilePicker.h>
#include <thread>

class LinuxFilePicker : public IFilePicker {
public:
  void PickOpenFile(
    std::vector<std::string> allowedExtensions,
    std::function<void(std::optional<std::string>)> onResult
  ) override {
    std::thread([onResult = std::move(onResult)] mutable {
      FILE* pipe = popen(
        "zenity --file-selection --file-filter='WAV files | *.wav' 2>/dev/null",
        "r"
      );

      if (pipe == nullptr) {
        onResult(std::nullopt);
        return;
      }

      char buffer[4096];
      std::string result;

      while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
      }

      const int exitCode = pclose(pipe);

      if (exitCode != 0 || result.empty()) {
        onResult(std::nullopt);
        return;
      }

      while (!result.empty() &&
             (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
      }

      onResult(result);
    }).detach();
  }
};

#endif //SYNTHEM_LINUXFILEPICKER_H

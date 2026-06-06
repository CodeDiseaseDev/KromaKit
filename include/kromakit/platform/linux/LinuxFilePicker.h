//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_LINUXFILEPICKER_H
#define SYNTHEM_LINUXFILEPICKER_H

#include <kromakit/platform/IFilePicker.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <functional>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <fcntl.h>


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class LinuxFilePicker : public IFilePicker {
public:
  void PickOpenFile(
    std::vector<std::string> allowedExtensions,
    std::function<void(std::optional<std::string>)> onResult
  ) override {
    std::thread(
      [
        allowedExtensions = std::move(allowedExtensions),
        onResult = std::move(onResult)
      ] mutable {
        onResult(PickFileBlocking(
          PickerMode::Open,
          allowedExtensions,
          {}
        ));
      }
    ).detach();
  }

  void PickSaveFile(
    std::vector<std::string> allowedExtensions,
    std::string defaultFileName,
    std::function<void(std::optional<std::string>)> onResult
  ) override {
    std::thread(
      [
        allowedExtensions = std::move(allowedExtensions),
        defaultFileName = std::move(defaultFileName),
        onResult = std::move(onResult)
      ] mutable {
        onResult(PickFileBlocking(
          PickerMode::Save,
          allowedExtensions,
          defaultFileName
        ));
      }
    ).detach();
  }

private:
  enum class PickerMode {
    Open,
    Save
  };

  static std::optional<std::string> PickFileBlocking(
    PickerMode mode,
    const std::vector<std::string>& allowedExtensions,
    const std::string& defaultFileName
  ) {
    int stdoutPipe[2];

    if (pipe(stdoutPipe) != 0) {
      return std::nullopt;
    }

    std::vector<std::string> args;
    args.push_back("zenity");
    args.push_back("--file-selection");

    if (mode == PickerMode::Save) {
      args.push_back("--save");
      args.push_back("--confirm-overwrite");

      if (!defaultFileName.empty()) {
        args.push_back("--filename=" + defaultFileName);
      }
    }

    const std::string filter = BuildZenityFilter(allowedExtensions);

    if (!filter.empty()) {
      args.push_back("--file-filter=" + filter);
    }

    args.push_back("--file-filter=All files | *");

    pid_t pid = fork();

    if (pid < 0) {
      close(stdoutPipe[0]);
      close(stdoutPipe[1]);
      return std::nullopt;
    }

    if (pid == 0) {
      close(stdoutPipe[0]);

      dup2(stdoutPipe[1], STDOUT_FILENO);

      close(stdoutPipe[1]);

      const int devNull = openDevNullWriteOnly();

      if (devNull >= 0) {
        dup2(devNull, STDERR_FILENO);
        close(devNull);
      }

      std::vector<char*> argv;
      argv.reserve(args.size() + 1);

      for (auto& arg : args) {
        argv.push_back(arg.data());
      }

      argv.push_back(nullptr);

      execvp("zenity", argv.data());

      _exit(127);
    }

    close(stdoutPipe[1]);

    std::string result;
    std::array<char, 4096> buffer {};

    while (true) {
      const ssize_t bytesRead = read(
        stdoutPipe[0],
        buffer.data(),
        buffer.size()
      );

      if (bytesRead > 0) {
        result.append(
          buffer.data(),
          static_cast<std::size_t>(bytesRead)
        );

        continue;
      }

      if (bytesRead == 0) {
        break;
      }

      if (errno == EINTR) {
        continue;
      }

      close(stdoutPipe[0]);
      WaitForChild(pid);
      return std::nullopt;
    }

    close(stdoutPipe[0]);

    const int exitCode = WaitForChild(pid);

    if (exitCode != 0 || result.empty()) {
      return std::nullopt;
    }

    TrimLineEnd(result);

    if (result.empty()) {
      return std::nullopt;
    }

    return result;
  }

  static int WaitForChild(pid_t pid) {
    int status = 0;

    while (waitpid(pid, &status, 0) < 0) {
      if (errno == EINTR) {
        continue;
      }

      return -1;
    }

    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }

    return -1;
  }

  static int openDevNullWriteOnly() {
    return open("/dev/null", O_WRONLY);
  }

  static std::string BuildZenityFilter(
    const std::vector<std::string>& allowedExtensions
  ) {
    std::vector<std::string> extensions;

    for (const auto& rawExtension : allowedExtensions) {
      auto extension = NormaliseExtension(rawExtension);

      if (!extension.has_value()) {
        continue;
      }

      if (std::find(
            extensions.begin(),
            extensions.end(),
            extension.value()
          ) != extensions.end()) {
        continue;
      }

      extensions.push_back(extension.value());
    }

    if (extensions.empty()) {
      return {};
    }

    std::string filter = "Allowed files |";

    for (const auto& extension : extensions) {
      filter += " *.";
      filter += extension;
    }

    return filter;
  }

  static std::optional<std::string> NormaliseExtension(
    std::string extension
  ) {
    while (!extension.empty() && extension.front() == '.') {
      extension.erase(extension.begin());
    }

    if (extension.empty() || extension.size() > 16) {
      return std::nullopt;
    }

    std::string result;
    result.reserve(extension.size());

    for (unsigned char c : extension) {
      if (std::isalnum(c) == 0) {
        return std::nullopt;
      }

      result.push_back(
        static_cast<char>(std::tolower(c))
      );
    }

    return result;
  }

  static void TrimLineEnd(std::string& text) {
    while (!text.empty() &&
           (text.back() == '\n' || text.back() == '\r')) {
      text.pop_back();
    }
  }
};

#endif // SYNTHEM_LINUXFILEPICKER_H
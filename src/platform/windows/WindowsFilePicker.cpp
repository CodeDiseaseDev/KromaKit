//
// Created by code on 6/1/26.
//

#ifdef _WIN32

#include <kromakit/platform/windows/WindowsFilePicker.h>

#include <windows.h>
#include <shobjidl.h>

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace {
  std::wstring ToWide(const std::string& text) {
    if (text.empty()) {
      return {};
    }

    const int size = MultiByteToWideChar(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      nullptr,
      0
    );

    std::wstring result(size, L'\0');

    MultiByteToWideChar(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      result.data(),
      size
    );

    if (!result.empty() && result.back() == L'\0') {
      result.pop_back();
    }

    return result;
  }

  std::string ToUtf8(const std::wstring& text) {
    if (text.empty()) {
      return {};
    }

    const int size = WideCharToMultiByte(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      nullptr,
      0,
      nullptr,
      nullptr
    );

    std::string result(size, '\0');

    WideCharToMultiByte(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      result.data(),
      size,
      nullptr,
      nullptr
    );

    if (!result.empty() && result.back() == '\0') {
      result.pop_back();
    }

    return result;
  }

  std::string NormaliseExtension(std::string extension) {
    extension.erase(
      std::remove_if(
        extension.begin(),
        extension.end(),
        [](unsigned char c) {
          return std::isspace(c) != 0;
        }
      ),
      extension.end()
    );

    if (!extension.empty() && extension.front() == '.') {
      extension.erase(extension.begin());
    }

    std::transform(
      extension.begin(),
      extension.end(),
      extension.begin(),
      [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
      }
    );

    return extension;
  }

  std::wstring BuildFilterName(
    const std::vector<std::string>& allowedExtensions
  ) {
    if (allowedExtensions.empty()) {
      return L"All Files";
    }

    std::wstring name = L"Allowed Files (";

    for (std::size_t i = 0; i < allowedExtensions.size(); ++i) {
      const std::string extension =
        NormaliseExtension(allowedExtensions[i]);

      if (extension.empty()) {
        continue;
      }

      if (i > 0) {
        name += L"; ";
      }

      name += L"*.";
      name += ToWide(extension);
    }

    name += L")";
    return name;
  }

  std::wstring BuildFilterSpec(
    const std::vector<std::string>& allowedExtensions
  ) {
    if (allowedExtensions.empty()) {
      return L"*.*";
    }

    std::wstring spec;

    for (const auto& rawExtension : allowedExtensions) {
      const std::string extension = NormaliseExtension(rawExtension);

      if (extension.empty()) {
        continue;
      }

      if (!spec.empty()) {
        spec += L";";
      }

      spec += L"*.";
      spec += ToWide(extension);
    }

    if (spec.empty()) {
      return L"*.*";
    }

    return spec;
  }

  class ComInit {
  public:
    ComInit() {
      result = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
      );
    }

    ~ComInit() {
      if (SUCCEEDED(result)) {
        CoUninitialize();
      }
    }

    bool Ok() const {
      return SUCCEEDED(result) || result == RPC_E_CHANGED_MODE;
    }

  private:
    HRESULT result = E_FAIL;
  };

  std::optional<std::string> GetShellItemPath(IShellItem* item) {
    if (item == nullptr) {
      return std::nullopt;
    }

    PWSTR filePath = nullptr;

    const HRESULT result = item->GetDisplayName(
      SIGDN_FILESYSPATH,
      &filePath
    );

    if (FAILED(result) || filePath == nullptr) {
      return std::nullopt;
    }

    std::wstring widePath(filePath);
    CoTaskMemFree(filePath);

    return ToUtf8(widePath);
  }

  std::optional<std::string> PickOpenFileBlocking(
    const std::vector<std::string>& allowedExtensions
  ) {
    ComInit com;

    if (!com.Ok()) {
      return std::nullopt;
    }

    IFileOpenDialog* dialog = nullptr;

    HRESULT result = CoCreateInstance(
      CLSID_FileOpenDialog,
      nullptr,
      CLSCTX_INPROC_SERVER,
      IID_PPV_ARGS(&dialog)
    );

    if (FAILED(result) || dialog == nullptr) {
      return std::nullopt;
    }

    const std::wstring filterName = BuildFilterName(allowedExtensions);
    const std::wstring filterSpec = BuildFilterSpec(allowedExtensions);

    const COMDLG_FILTERSPEC filters[] = {
      { filterName.c_str(), filterSpec.c_str() },
      { L"All Files (*.*)", L"*.*" }
    };

    dialog->SetFileTypes(2, filters);
    dialog->SetFileTypeIndex(1);
    dialog->SetTitle(L"Open File");

    result = dialog->Show(nullptr);

    if (FAILED(result)) {
      dialog->Release();
      return std::nullopt;
    }

    IShellItem* item = nullptr;
    result = dialog->GetResult(&item);

    if (FAILED(result) || item == nullptr) {
      dialog->Release();
      return std::nullopt;
    }

    auto path = GetShellItemPath(item);

    item->Release();
    dialog->Release();

    return path;
  }

  std::optional<std::string> PickSaveFileBlocking(
    const std::vector<std::string>& allowedExtensions,
    const std::string& defaultFileName
  ) {
    ComInit com;

    if (!com.Ok()) {
      return std::nullopt;
    }

    IFileSaveDialog* dialog = nullptr;

    HRESULT result = CoCreateInstance(
      CLSID_FileSaveDialog,
      nullptr,
      CLSCTX_INPROC_SERVER,
      IID_PPV_ARGS(&dialog)
    );

    if (FAILED(result) || dialog == nullptr) {
      return std::nullopt;
    }

    const std::wstring filterName = BuildFilterName(allowedExtensions);
    const std::wstring filterSpec = BuildFilterSpec(allowedExtensions);

    const COMDLG_FILTERSPEC filters[] = {
      { filterName.c_str(), filterSpec.c_str() },
      { L"All Files (*.*)", L"*.*" }
    };

    dialog->SetFileTypes(2, filters);
    dialog->SetFileTypeIndex(1);
    dialog->SetTitle(L"Save File");

    if (!defaultFileName.empty()) {
      dialog->SetFileName(ToWide(defaultFileName).c_str());
    }

    if (!allowedExtensions.empty()) {
      const std::string extension = NormaliseExtension(allowedExtensions.front());

      if (!extension.empty()) {
        dialog->SetDefaultExtension(ToWide(extension).c_str());
      }
    }

    result = dialog->Show(nullptr);

    if (FAILED(result)) {
      dialog->Release();
      return std::nullopt;
    }

    IShellItem* item = nullptr;
    result = dialog->GetResult(&item);

    if (FAILED(result) || item == nullptr) {
      dialog->Release();
      return std::nullopt;
    }

    auto path = GetShellItemPath(item);

    item->Release();
    dialog->Release();

    return path;
  }
}

void WindowsFilePicker::PickOpenFile(
  std::vector<std::string> allowedExtensions,
  std::function<void(std::optional<std::string>)> onResult
) {
  std::thread(
    [
      allowedExtensions = std::move(allowedExtensions),
      onResult = std::move(onResult)
    ]() mutable {
      auto result = PickOpenFileBlocking(allowedExtensions);

      if (onResult) {
        onResult(std::move(result));
      }
    }
  ).detach();
}

void WindowsFilePicker::PickSaveFile(
  std::vector<std::string> allowedExtensions,
  std::string defaultFileName,
  std::function<void(std::optional<std::string>)> onResult
) {
  std::thread(
    [
      allowedExtensions = std::move(allowedExtensions),
      defaultFileName = std::move(defaultFileName),
      onResult = std::move(onResult)
    ]() mutable {
      auto result = PickSaveFileBlocking(
        allowedExtensions,
        defaultFileName
      );

      if (onResult) {
        onResult(std::move(result));
      }
    }
  ).detach();
}

#endif

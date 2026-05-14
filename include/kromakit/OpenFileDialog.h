//
// Created by code on 4/11/26.
//

#ifndef DUI1_OPENFILEDIALOG_H
#define DUI1_OPENFILEDIALOG_H

#include <filesystem>
#include <thread>

#include <kromakit/Panel.h>
#include <kromakit/Utils.h>
namespace fs = std::filesystem;

#include <kromakit/Application.h>

// class OpenFileDialog;

class OpenFileDialog :
  public Control {

private:
  class FileList :
    public Control {




    class File :
      public Control {

      fs::path path;

    public:
      bool selected = false;

      std::function<void()> OnClicked = nullptr;

      File(fs::path path) {
        // size = {200, 30};
        // SetContent(text);
        this->path = path;
        // background = ColorFromRGB(25);
        foreground = ColorFromRGB(255);
      }

      const std::string get_str() const {
        return std::string(is_directory(path) ? "D" : "F") + " " + path.filename().string(); //
      }

      void OnRender(Graphics *rendTarget) override {
        rendTarget->FillControlBackground(this);


        constexpr float spacing = 5;
        rendTarget->RenderText(
          get_str().c_str(), font, foreground, spacing, spacing);
      }
      void DoLayout(Graphics *renderTarget) override {
        background =
          isSelected ? ColorFromRGB(45) :
                       ColorFromRGB(0, 0);

        constexpr float spacing = 5;
        size.height = renderTarget->CalculateTextSize(
          get_str().c_str(), font).height + spacing * 2;
      }

      std::optional<std::string> GetControlName() const override { return "File"; }
      const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

      void OnMouseDown() override {
        if (OnClicked != nullptr) {
          OnClicked();
        }
      }
    };

    std::optional<std::string> GetControlName() const override { return "FileList"; }
    const std::vector<std::string> GetDebugOverlayItems() override { return {}; }




    std::vector<File*> files;

  public:



    void RenderDir(std::string directory) {
      files.clear();
      Children.clear();

      auto file_list = fs::directory_iterator(directory);
      std::vector<fs::directory_entry> entries;

      for (const auto& entry : fs::directory_iterator(directory)) {
        entries.push_back(entry);
      }

      std::sort(entries.begin(), entries.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {

          bool aDir = a.is_directory();
          bool bDir = b.is_directory();

          // folders first
          if (aDir != bDir)
            return aDir > bDir;

          // then alphabetical
          return a.path().filename().string() <
                 b.path().filename().string();
        }
      );

      for (const auto & entry : entries) {

        auto file = std::make_unique<File>(entry);
        file->isSelected = false;
        File* filePtr = file.get(); // 👈 capture this instead


        file->OnClicked = [this, filePtr]() {
          for (const auto& f : files) {
            f->isSelected = false;
          }
          filePtr->isSelected = true;
        };

        files.push_back(filePtr);
        AddControl(std::move(file));

        Logging::Log("%s\n", entry.path().string().c_str());
      }

      for (const auto & entry : fs::directory_iterator(directory)) {

      }
    }

    void OnRender(Graphics *rendTarget) override {

    }
    void DoLayout(Graphics *renderTarget) override {


      constexpr float spacing = 3;
      DUIPoint point = {0,0};

      for (const auto &file : files) {
        file->location = point;
        file->size.width = size.width;

        point.y += file->size.height + spacing;
      }


    }

  };

  class DialogOverlay :
    public Control {
  std::optional<std::string> GetControlName() const override { return "DialogOverlay"; }
    const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

    void OnRender(Graphics *rendTarget) override {
      rendTarget->FillControlBackground(this);
    }
    void DoLayout(Graphics *renderTarget) override {
      size = parent->size;
    }
  };

public:
  OpenFileDialog() {
    fileList = CreateControl<FileList>();
    fileList->RenderDir("/home/code");

    background = ColorFromRGB(20);
  }

  FileList* fileList = nullptr;

  std::optional<std::string> GetControlName() const override { return "OpenFileDialog"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  void OnRender(Graphics *rendTarget) override {
    rendTarget->FillControlBackground(this);
  }
  void DoLayout(Graphics *renderTarget) override {
    size = {400, 300};

    fileList->size = size;

    location = {
      parent->size.width / 2 - size.width / 2,
      parent->size.height / 2 - size.height / 2
    };
  }

  static void ShowFileDialog(Control* target) {
    auto overlay = std::make_unique<DialogOverlay>();


    // overlay->AddControl(std::make_unique<OpenFileDialog>());
    overlay->location = {0,0};
    overlay->size = target->size;
    overlay->background = ColorFromRGB(0, 200);

    auto ofd = std::make_unique<OpenFileDialog>();
    // overlay->AddControl(std::move(ofd));
    //
    // target->AddControl(std::move(overlay));
  }
};





#endif //DUI1_OPENFILEDIALOG_H

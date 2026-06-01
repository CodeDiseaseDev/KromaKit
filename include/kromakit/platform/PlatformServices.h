//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_PLATFORMSERVICES_H
#define SYNTHEM_PLATFORMSERVICES_H
#include "IFilePicker.h"

#include <memory>

class PlatformServices {
public:
  static std::shared_ptr<IFilePicker> FilePicker();

  static void DisplayMessageBox_(std::string title, std::string message);
};

#endif //SYNTHEM_PLATFORMSERVICES_H

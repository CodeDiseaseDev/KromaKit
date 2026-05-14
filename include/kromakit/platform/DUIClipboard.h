//
// Created by code on 5/10/26.
//

#ifndef TEST_IOS_APP_DUICLIPBOARD_H
#define TEST_IOS_APP_DUICLIPBOARD_H
#include <string>

class DUIClipboard {
public:
  static std::string GetText();
  static void SetText(std::string str);
};

#endif //TEST_IOS_APP_DUICLIPBOARD_H

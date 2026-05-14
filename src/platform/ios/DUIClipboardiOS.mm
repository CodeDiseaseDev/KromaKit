#import <UIKit/UIKit.h>

#include <kromakit/platform/DUIClipboard.h>

std::string DUIClipboard::GetText() {
  NSString* text = [UIPasteboard generalPasteboard].string;

  if (text == nil)
    return {};

  return std::string([text UTF8String]);
}

void DUIClipboard::SetText(std::string str) {
  NSString* text = [NSString stringWithUTF8String:str.c_str()];

  if (text == nil)
    text = @"";

  [UIPasteboard generalPasteboard].string = text;
}

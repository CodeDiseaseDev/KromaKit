#import <AppKit/AppKit.h>

#include <kromakit/platform/DUIClipboard.h>

std::string DUIClipboard::GetText() {
  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];

  NSString* text =
    [pasteboard stringForType:NSPasteboardTypeString];

  if (text == nil)
    return {};

  return std::string([text UTF8String]);
}

void DUIClipboard::SetText(std::string str) {
  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];

  [pasteboard clearContents];

  NSString* text =
    [NSString stringWithUTF8String:str.c_str()];

  if (text == nil)
    text = @"";

  [pasteboard setString:text
                forType:NSPasteboardTypeString];
}

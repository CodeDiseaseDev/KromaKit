//
// Created by code on 5/6/26.
//

#import <Foundation/Foundation.h>

#include <kromakit/platform/macos/FontLoaderMacOS.h>

#include <string>

#include <nanovg.h>
#include <kromakit/DUIFont.h>
#include <kromakit/platform/PlatformResourceLoader.h>

namespace {

NSString* ResolveFontPath(NSString* directory, NSString* filename) {
  std::string relativePath =
    std::string(directory.UTF8String) + "/" +
    std::string(filename.UTF8String) + ".ttf";

  std::string resolvedPath =
    PlatformResourceLoader::ResolveResourcePath(relativePath);

  NSString* path = [NSString stringWithUTF8String:resolvedPath.c_str()];
  if (path != nil && [[NSFileManager defaultManager] fileExistsAtPath:path]) {
    return path;
  }

  return nil;
}

bool RegisterFontFace(
  NVGcontext* ctx,
  NSString* family,
  FontWeight weight,
  FontStyle style,
  NSString* directory,
  NSString* filename
) {
  if (ctx == nullptr) {
    return false;
  }

  NSString* path = ResolveFontPath(directory, filename);
  if (path == nil) {
    NSLog(@"Failed to locate macOS font: %@/%@.ttf", directory, filename);
    return false;
  }

  NSString* faceName = filename;
  int fontId = nvgCreateFont(
    ctx,
    faceName.UTF8String,
    path.UTF8String
  );

  if (fontId < 0) {
    NSLog(@"Failed to register NanoVG font '%@' from %@", faceName, path);
    return false;
  }

  FontRegistry::Register(
    std::string(family.UTF8String),
    weight,
    style,
    std::string(faceName.UTF8String)
  );

  return true;
}

} // namespace

void LoadBundledFontsMacOS(NVGcontext* ctx) {
  if (ctx == nullptr) {
    return;
  }

  RegisterFontFace(
    ctx,
    @"Poppins",
    FontWeight::Regular,
    FontStyle::Normal,
    @"fonts/Poppins",
    @"Poppins-Regular"
  );

  RegisterFontFace(
    ctx,
    @"Ubuntu",
    FontWeight::Regular,
    FontStyle::Normal,
    @"fonts/Ubuntu",
    @"Ubuntu-Regular"
  );

  RegisterFontFace(
    ctx,
    @"JetBrainsMono",
    FontWeight::Regular,
    FontStyle::Normal,
    @"fonts/JetBrainsMono",
    @"JetBrainsMonoNerdFontMono-Regular"
  );

  FontRegistry::Register(
    "default",
    FontWeight::Regular,
    FontStyle::Normal,
    "Ubuntu-Regular"
  );
}

//
// Created by code on 4/26/26.
//

#pragma once

#ifndef TEST_IOS_APP_DIRECTUI_PLATFORM_IOS_FONT_LOADER_H
#define TEST_IOS_APP_DIRECTUI_PLATFORM_IOS_FONT_LOADER_H

#import <Foundation/Foundation.h>

struct NVGcontext;

#include <kromakit/DUIFont.h>

@interface FontLoader : NSObject

- (instancetype)initWithContext:(NVGcontext *)ctx;

- (BOOL)loadNanoVGFontFace:(NSString *)faceName
                  filename:(NSString *)filename
                 directory:(NSString *)directory;

- (BOOL)registerFontFamily:(NSString *)family
                     style:(NSString *)style
                    weight:(FontWeight)weight
                 fontStyle:(FontStyle)fontStyle
                 directory:(NSString *)directory
                  filename:(NSString *)filename;

- (void)loadBundledFonts;

@end

#endif // TEST_IOS_APP_DIRECTUI_PLATFORM_IOS_FONT_LOADER_H

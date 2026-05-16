//
// Created by code on 4/26/26.
//

//#import "FontLoader.h"
#import <kromakit/platform/ios/FontLoader.h>

#include <string>
#include <nanovg.h>
#include <kromakit/platform/PlatformResourceLoader.h>

@implementation FontLoader {
    NVGcontext *_ctx;
}

- (instancetype)initWithContext:(NVGcontext *)ctx {
    self = [super init];

    if (self) {
        _ctx = ctx;
    }

    return self;
}

- (NSString *)fontFaceNameForFamily:(NSString *)family
                              style:(NSString *)style {
    if (style.length == 0) {
        return family;
    }

    return [NSString stringWithFormat:@"%@-%@", family, style];
}

- (BOOL)loadNanoVGFontFace:(NSString *)faceName
                  filename:(NSString *)filename
                 directory:(NSString *)directory {
    if (!_ctx) {
        NSLog(@"Cannot load font '%@': NanoVG context is null.", faceName);
        return NO;
    }

    std::string relativePath =
        std::string(directory.UTF8String) + "/" +
        std::string(filename.UTF8String) + ".ttf";

    std::string resolvedPath =
        PlatformResourceLoader::ResolveResourcePath(relativePath);

    NSString *fontPath =
        [NSString stringWithUTF8String:resolvedPath.c_str()];

    if (!fontPath ||
        ![[NSFileManager defaultManager] fileExistsAtPath:fontPath]) {
        NSLog(@"Cannot load font '%@': %@.ttf not found in %@",
              faceName,
              filename,
              directory);
        return NO;
    }

    int fontId = nvgCreateFont(
        _ctx,
        faceName.UTF8String,
        fontPath.UTF8String
    );

    if (fontId == -1) {
        NSLog(@"Failed to register NanoVG font '%@' from %@",
              faceName,
              fontPath);
        return NO;
    }

    NSLog(@"Loaded NanoVG font '%@' from %@", faceName, fontPath);
    return YES;
}

- (BOOL)registerFontFamily:(NSString *)family
                     style:(NSString *)style
                    weight:(FontWeight)weight
                 fontStyle:(FontStyle)fontStyle
                 directory:(NSString *)directory
                  filename:(NSString *)filename {
    NSString *faceName = [self fontFaceNameForFamily:family style:style];

    if (![self loadNanoVGFontFace:faceName
                         filename:filename
                        directory:directory]) {
        return NO;
    }

    FontRegistry::Register(
        std::string(family.UTF8String),
        weight,
        fontStyle,
        std::string(faceName.UTF8String)
    );

    return YES;
}

- (void)registerPoppinsFonts {
    NSString *family = @"Poppins";
    NSString *directory = @"fonts/Poppins";

    [self registerFontFamily:family
                       style:@"Thin"
                      weight:FontWeight::Thin
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Thin"];

    [self registerFontFamily:family
                       style:@"ThinItalic"
                      weight:FontWeight::Thin
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-ThinItalic"];

    [self registerFontFamily:family
                       style:@"ExtraLight"
                      weight:FontWeight::ExtraLight
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-ExtraLight"];

    [self registerFontFamily:family
                       style:@"ExtraLightItalic"
                      weight:FontWeight::ExtraLight
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-ExtraLightItalic"];

    [self registerFontFamily:family
                       style:@"Light"
                      weight:FontWeight::Light
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Light"];

    [self registerFontFamily:family
                       style:@"LightItalic"
                      weight:FontWeight::Light
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-LightItalic"];

    [self registerFontFamily:family
                       style:@"Regular"
                      weight:FontWeight::Regular
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Regular"];

    [self registerFontFamily:family
                       style:@"Italic"
                      weight:FontWeight::Regular
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-Italic"];

    [self registerFontFamily:family
                       style:@"Medium"
                      weight:FontWeight::Medium
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Medium"];

    [self registerFontFamily:family
                       style:@"MediumItalic"
                      weight:FontWeight::Medium
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-MediumItalic"];

    [self registerFontFamily:family
                       style:@"SemiBold"
                      weight:FontWeight::SemiBold
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-SemiBold"];

    [self registerFontFamily:family
                       style:@"SemiBoldItalic"
                      weight:FontWeight::SemiBold
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-SemiBoldItalic"];

    [self registerFontFamily:family
                       style:@"Bold"
                      weight:FontWeight::Bold
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Bold"];

    [self registerFontFamily:family
                       style:@"BoldItalic"
                      weight:FontWeight::Bold
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-BoldItalic"];

    [self registerFontFamily:family
                       style:@"ExtraBold"
                      weight:FontWeight::ExtraBold
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-ExtraBold"];

    [self registerFontFamily:family
                       style:@"ExtraBoldItalic"
                      weight:FontWeight::ExtraBold
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-ExtraBoldItalic"];

    [self registerFontFamily:family
                       style:@"Black"
                      weight:FontWeight::Black
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Poppins-Black"];

    [self registerFontFamily:family
                       style:@"BlackItalic"
                      weight:FontWeight::Black
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Poppins-BlackItalic"];
}

- (void)registerUbuntuFonts {
    NSString *family = @"Ubuntu";
    NSString *directory = @"fonts/Ubuntu";

    [self registerFontFamily:family
                       style:@"Light"
                      weight:FontWeight::Light
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Ubuntu-Light"];

    [self registerFontFamily:family
                       style:@"LightItalic"
                      weight:FontWeight::Light
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Ubuntu-LightItalic"];

    [self registerFontFamily:family
                       style:@"Regular"
                      weight:FontWeight::Regular
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Ubuntu-Regular"];

    [self registerFontFamily:family
                       style:@"Italic"
                      weight:FontWeight::Regular
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Ubuntu-Italic"];

    [self registerFontFamily:family
                       style:@"Medium"
                      weight:FontWeight::Medium
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Ubuntu-Medium"];

    [self registerFontFamily:family
                       style:@"MediumItalic"
                      weight:FontWeight::Medium
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Ubuntu-MediumItalic"];

    [self registerFontFamily:family
                       style:@"Bold"
                      weight:FontWeight::Bold
                   fontStyle:FontStyle::Normal
                   directory:directory
                    filename:@"Ubuntu-Bold"];

    [self registerFontFamily:family
                       style:@"BoldItalic"
                      weight:FontWeight::Bold
                   fontStyle:FontStyle::Italic
                   directory:directory
                    filename:@"Ubuntu-BoldItalic"];
}

- (void)registerJetBrainsMonoFonts {
    [self registerFontFamily:@"JetBrainsMono"
                       style:@"Regular"
                      weight:FontWeight::Regular
                   fontStyle:FontStyle::Normal
                   directory:@"fonts/JetBrainsMono"
                    filename:@"JetBrainsMonoNerdFontMono-Regular"];
}

- (void)registerLegacyAliases {
    FontRegistry::Register(
        "default",
        FontWeight::Regular,
        FontStyle::Normal,
        "Ubuntu-Regular"
    );

    FontRegistry::Register(
        "Poppins",
        FontWeight::Regular,
        FontStyle::Normal,
        "Poppins-Regular"
    );

    FontRegistry::Register(
        "JetBrainsMono",
        FontWeight::Regular,
        FontStyle::Normal,
        "JetBrainsMono-Regular"
    );
}

- (void)loadBundledFonts {
    [self registerPoppinsFonts];
    [self registerUbuntuFonts];
    [self registerJetBrainsMonoFonts];

    [self registerLegacyAliases];
}

@end

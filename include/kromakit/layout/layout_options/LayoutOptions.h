//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_LAYOUTOPTIONS_H
#define TEST_IOS_APP_LAYOUTOPTIONS_H

#include <kromakit/layout/layout_options/GridLayoutOptions.h>
#include <kromakit/graphics/Graphics.h>
#include <kromakit/layout/layout_options/LayoutLength.h>
#include <kromakit/layout/layout_options/LayoutAlignment.h>


struct LayoutOptions {
  LayoutLength width = LayoutLength::Auto();
  LayoutLength height = LayoutLength::Auto();

  DUIInsets margin = {};
  LayoutAlignment horizontalAlignment = LayoutAlignment::Stretch;
  LayoutAlignment verticalAlignment = LayoutAlignment::Stretch;

  GridLayoutOptions gridLayout;
};

#endif //TEST_IOS_APP_LAYOUTOPTIONS_H

#pragma once

#include <kromakit/presentation/FloatingWindowControl.h>
#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/styling/controls/CheckboxStyle.h>
#include <kromakit/styling/controls/FPSLabelStyle.h>
#include <kromakit/styling/controls/GroupBoxStyle.h>
#include <kromakit/styling/controls/HStackPanelStyle.h>
#include <kromakit/styling/controls/HorizontalScrollBarStyle.h>
#include <kromakit/styling/controls/LabelStyle.h>
#include <kromakit/styling/controls/ModalPresentationStyle.h>
#include <kromakit/styling/controls/PanelStyle.h>
#include <kromakit/styling/controls/ScrollBarStyle.h>
#include <kromakit/styling/controls/SeparatorStyle.h>
#include <kromakit/styling/controls/SwipeActionButtonStyle.h>
#include <kromakit/styling/controls/TextBlockStyle.h>
#include <kromakit/styling/controls/TextBoxStyle.h>
#include <kromakit/styling/controls/VStackPanelStyle.h>
#include <kromakit/styling/controls/ContextMenuStyle.h>
#include <kromakit/styling/controls/FloatingWindowControlStyle.h>
#include <kromakit/styling/controls/IconButtonStyle.h>

namespace DUIDefaultStyles {



  inline void RegisterAll() {
    PanelStyles::Register();
    LabelStyles::Register();
    TextBlockStyles::Register();
    ButtonStyles::Register();
    // TextBoxStyles::Register(); // TextBox control to be implemented for iOS
    SeparatorStyles::Register();
    ScrollBarStyles::Register();
    HorizontalScrollBarStyles::Register();
    HStackPanelStyles::Register();
    VStackPanelStyles::Register();
    CheckboxStyles::Register();
    GroupBoxStyles::Register();
    ModalPresentationStyles::Register();
    SwipeActionButtonStyles::Register();
    FPSLabelStyles::Register();
    ContextMenuStyle::Register();

    FloatingWindowControlStyles::Register();

    BorderBaseStyles::Register();
    ForegroundBaseStyles::Register();
    SurfaceBaseStyles::Register();

    IconButtonStyles::Register();

    Logging::Log("DUIDefaultStyles Registered.\n");
  }
}

# KromaKit

A lightweight C++ UI framework for building custom, animated, cross-platform interfaces.

KromaKit is a retained-mode UI framework built around custom controls, flexible layout, styled rendering, and platform backends for desktop and Apple targets. It started life as the UI engine behind Synthem, then grew into its own standalone framework.

It is designed for apps that want full control over their interface without dragging around a heavyweight native widget toolkit.

Think: custom music tools, creative apps, inspectors, editors, embedded-style interfaces, experimental UI, and anything where “just use the platform button” is not the vibe.

---

## Status

KromaKit is under active development.

The framework is usable for real experiments and internal apps, but the public API is still evolving. Expect rough edges, missing docs, and occasional architecture earthquakes.

Current focus areas:

* Cross-platform rendering
* Custom controls and layout
* Style system
* Overlay and modal UI
* Touch, mouse, keyboard, and gesture input
* iOS/macOS Metal support
* Linux/Windows OpenGL support
* Smooth animated interfaces

---

## Features

* Custom retained-mode control tree
* C++ control classes with overridable layout, rendering, and input behavior
* CSS-ish style registry
* Flexible layout panels
* Scrollable views
* Context menus and overlays
* Modal and floating UI support
* Touch and pointer input
* Keyboard input abstraction
* SVG icon support
* Font Awesome icon registry support
* NanoVG-based rendering
* Metal backend for iOS/macOS
* OpenGL backend for Linux/Windows
* Platform capability abstraction
* Debug tree and developer tooling hooks

---

## Platforms

KromaKit currently targets:

* Linux
* Windows
* macOS
* iOS

Rendering backends:

* NanoVG + OpenGL for Linux/Windows
* NanoVG + Metal for iOS/macOS

The goal is to keep application UI code mostly platform-independent while still allowing platform-specific polish where needed.

---

## Design Philosophy

KromaKit is built around a few ideas:

### Custom UI should still be structured

KromaKit is not immediate-mode UI. Controls exist as objects, live in a tree, own state, receive input, and render themselves.

### Controls should be composable

A button, panel, editor, overlay, menu, or custom sequencer widget is just a control. Complex interfaces are built by nesting controls and panels.

### Styling should be reusable

Controls can opt into named styles instead of hardcoding every color, radius, margin, and visual state.

### Rendering should be portable

The framework abstracts enough platform details to share UI code across desktop and mobile targets, while keeping rendering fast and low-level.

### The app owns the vibe

KromaKit does not try to look like UIKit, AppKit, WinUI, GTK, or Qt. It gives you the machinery to build your own visual language.

---

## Basic Architecture

A KromaKit app is generally built from:

* An application host
* A window or platform host
* A root control
* A tree of child controls
* A renderer backend
* Platform input plumbing

Controls inherit from a base `Control` class and can implement their own layout, drawing, and input behavior.

A simplified mental model:

```
Application
  Window / Platform Host
    Root Control
      Panel
        Button
        Label
        Custom Control
        Overlay Layer
```

Each control is responsible for its own behavior, while the framework handles tree traversal, clipping, transforms, invalidation, and event routing.

---

## Example Control Setup

Example shape:

```
class MyApp : public Application {
protected:
  void UserInitialise() override {
    auto root = std::make_unique<Panel>();

    auto title = std::make_unique<Label>();
    title->SetText("Hello from KromaKit");
    title->UseStyle("Label.Title");

    auto button = std::make_unique<Button>();
    button->SetText("Press me");
    button->UseStyle("Button.Primary");

    root->AddChild(std::move(title));
    root->AddChild(std::move(button));

    GetWindow().SetRootControl(std::move(root));
  }
};
```

KromaKit is intentionally code-first. There is no GUI designer. The UI tree is built directly in C++.

---

## Styling

KromaKit includes a named style registry.

Styles can be registered once and applied to controls by name:

```
DUIStyleRegistry::Register<Button>("Button.Primary", [](Button& button) {
  button.SetCornerRadius(14);
  button.SetPadding({ 14, 8 });
  button.SetBackgroundColor(Colors::Accent);
  button.SetForegroundColor(Colors::TextPrimary);
});

button.UseStyle("Button.Primary");
```

The style system is still evolving, but the goal is to make visual design reusable without hiding the fact that this is still C++.

Current style concepts include:

* Surface styles
* Foreground styles
* Label styles
* Button styles
* Context menu styles
* Semantic color tokens
* Control-specific style hooks

---

## Layout

KromaKit has layout panels for arranging controls.

Current and planned layout primitives include:

* Panel
* Horizontal stack panel
* Vertical stack panel
* Scrollable views
* Grid layout panels
* Free-scroll panels
* Keyboard-aware mobile panels

Grid layout supports layout lengths such as:

* Auto
* Fixed
* Fill
* Percent

Example idea:

```
GridLayoutPanel
  Row: Auto
  Row: Fill
  Column: Fixed(240)
  Column: Fill
```

This makes it possible to build editor-style interfaces, sidebars, toolbars, inspectors, and scrollable content areas.

---

## Overlays and Modals

KromaKit includes an overlay system for UI that floats above normal content.

Useful for:

* Dialogs
* Context menus
* Popovers
* Floating panels
* Parameter adjustment overlays
* Mobile sheets
* Debug overlays

Overlay rendering can support blurred/dimmed backgrounds depending on backend support.

Example use cases:

* Long-press context menus
* Modal editor panels
* Inspector popups
* Large touch-friendly parameter adjustment controls

---

## Input

KromaKit supports platform-independent input events for:

* Pointer movement
* Pointer down/up
* Mouse buttons
* Touch
* Wheel scrolling
* Keyboard input
* Text input
* Hold interactions
* Swipe/momentum gestures

The input model is designed to work across both desktop and mobile.

Desktop can use mouse, keyboard, scroll wheel, and right-click context menus.

Mobile can use touch, long press, haptics, keyboard-aware panels, and overlay-style interactions.

---

## SVG and Icons

KromaKit supports SVG rendering through NanoSVG.

The framework includes work toward:

* SVG resource caching
* Runtime icon recoloring
* Font Awesome SVG integration
* Generated icon registries
* Style-aware icon colors

Example icon usage shape:

```
auto icon = std::make_unique<SvgIcon>();
icon->SetIcon(DUIIcons::FontAwesome::Solid::Play);
icon->SetColor(Colors::ForegroundPrimary);
```

The goal is to make icons lightweight, scalable, and easy to theme.

---

## Rendering

KromaKit uses NanoVG as the high-level 2D drawing layer.

Backends currently include:

* OpenGL backend for Linux and Windows
* Metal backend for iOS and macOS

The rendering layer handles:

* Shapes
* Text
* SVG paths
* Clipping
* Rounded rectangles
* Transforms
* Backdrop effects
* Control rendering traversal

Some rendering features are still being expanded, especially around advanced clipping, varied border thicknesses, rounded clipping, and composited effects.

---

## Platform Abstraction

KromaKit has a platform abstraction layer that describes the current environment.

Example platform concepts:

* Form factor

  * Desktop
  * Mobile
  * Tablet
  * TV
  * Embedded

* Operating system

  * Linux
  * Windows
  * macOS
  * iOS

* Renderer

  * OpenGL
  * Metal
  * Direct3D
  * Vulkan

* Capabilities

  * Touch
  * Mouse
  * Keyboard
  * Pen
  * Haptics
  * Scale factor

This allows UI code to adapt without being completely rewritten per platform.

---

## Developer Tools

KromaKit includes debug and developer tooling hooks such as:

* Control tree printing
* Debug overlays
* Layout inspection hooks
* Performance overlays
* Logging helpers
* Control name lookup
* Devtools UI experiments

These tools are still evolving, but the direction is clear: debugging a custom UI framework should not feel like reading tea leaves in a thunderstorm.

---

## Example Use Cases

KromaKit is a good fit for:

* Custom audio tools
* Synthesizers
* Sequencers
* Visual editors
* Debug inspectors
* Bundle inspectors
* Creative tools
* Custom dashboards
* Touch-first experimental apps
* Desktop/mobile hybrid tools

It is probably not the best fit if you want a standard native business app with platform-default widgets.

---

## Relationship to Synthem

KromaKit was originally developed as the UI framework behind Synthem, an experimental audio/synth/sequencer app.

Synthem needed a UI system that could support:

* Custom sequencer grids
* Pattern editors
* Playlist editors
* Animated controls
* Touch-friendly parameter editing
* iOS Metal rendering
* Desktop OpenGL rendering
* Custom styling
* High refresh-rate UI

KromaKit exists because those requirements were too specific for a generic UI toolkit.

---

## Build Notes

Build tooling is still project-specific and may change.

Typical dependencies include:

* C++20 or newer compiler
* CMake or Make-based build setup
* NanoVG
* NanoSVG
* GLFW for desktop OpenGL hosts
* OpenGL libraries on Linux/Windows
* Metal frameworks on Apple platforms
* Platform SDKs for iOS/macOS builds

Linux desktop builds generally require:

```
glfw3
OpenGL
pkg-config
clang or gcc
```

Windows builds currently use a MinGW-style cross-build path in the SynthemKit environment.

Apple builds use Objective-C++ platform hosts with Metal.

---

## Repository Layout

A typical KromaKit layout may look like:

```
include/
  kromakit/
    controls/
    graphics/
    layout/
    platform/
    styles/
    input/

src/
  controls/
  graphics/
  layout/
  platform/
  styles/
  input/

examples/
  basic/
  controls/
  overlays/

tools/
  icon-generation/
  demo-renderer/

tests/
```

Exact folders may change as the framework is cleaned up and separated further from SynthemKit.

---

## Roadmap

Possible future work:

* Cleaner public API
* Better documentation
* More examples
* Demo renderer for README animations
* Improved layout invalidation
* More controls
* Better text input
* More complete keyboard navigation
* Improved accessibility story
* Better GPU-backed compositing
* Rounded clipping improvements
* More advanced border rendering
* Better packaging for external projects
* CI builds across platforms
* Public example apps
* More stable style/property system

---

## Demo Renderer Idea

A planned tool is a deterministic demo renderer for creating README animations.

The idea is to define UI demos as classes:

```
IControlDemo
  CreateControl()
  ConfigureScene()
  BuildTimeline()
```

Then render a control tree frame-by-frame through the real KromaKit renderer.

This would allow generated GIF/WebP/MP4 demos showing:

* Controls animating
* Context menus
* Parameter overlays
* Sequencer interactions
* Style transitions
* Touch gestures

README animations would be rendered by the actual framework, not screen recordings.

That is probably unnecessary.

Which is exactly why it sounds fun.

---

## License

License to be decided.

Possible options:

* MIT
* Apache-2.0
* GPLv3 with commercial licensing
* Source-available private license

The licensing model will depend on whether KromaKit becomes a public framework, a portfolio project, or part of a future commercial product ecosystem.

---

## Contributing

Not ready yet.

KromaKit is still being shaped, renamed, separated, and cleaned up. Once the public API settles down, contribution guidelines can be added.

For now, expect dragons, sharp edges, and at least one file that looks like it was written during a caffeine eclipse.

---

## Why?

Because sometimes the UI is the product.

Because creative tools deserve custom interfaces.

Because building a framework is unreasonable, educational, painful, and weirdly addictive.

Because “just use Qt” was not the answer this time.

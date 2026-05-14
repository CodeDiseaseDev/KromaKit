//
// Created by code on 5/6/26.
//

#include <kromakit/presentation/FloatingWindowControl.h>

#include <kromakit/graphics/svg/FontAwesomeIcons.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>
#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/styling/controls/FloatingWindowControlStyle.h>

FloatingWindowControl::FloatingWindowControl() {
  size = { 400,600 };



  gridLayoutPanel = CreateControl<GridLayoutPanel>();
  gridLayoutPanel->SetGridColumns({ LayoutLength::Fill() });
  gridLayoutPanel->SetGridRows({
    LayoutLength::Fixed(35.0f),
    LayoutLength::Fill()
  });

  titlePanel = gridLayoutPanel->CreateLayoutChild<GridLayoutPanel>(
    GridLayoutOptions { 0, 0, 1, 1 }
  );
  contentPanel = gridLayoutPanel->CreateLayoutChild<Panel>(
    GridLayoutOptions { 0, 1, 1, 1 }
  );

  titlePanel->SetGridRows({
    LayoutLength::Fill()
  });
  titlePanel->SetGridColumns({
    LayoutLength::Fixed(12.0f),
    LayoutLength::Fill(),
    LayoutLength::Fixed(40.0f)
  });

  windowLabel = titlePanel->CreateLayoutChild<Label>(
    GridLayoutOptions { 1, 0, 1, 1 }
  );
  windowLabel->autoHeight = windowLabel->autoWidth = false;

  closeButton = titlePanel->CreateLayoutChild<IconButton>(
    GridLayoutOptions { 2, 0, 1, 1 }, DUIIcons::FontAwesome::Solid::Slash
  );
  closeButton->SetRadius(8);
  closeButton->UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);
  closeButton->OnClick = [this] {
    if (OnCloseRequested != nullptr) {
      OnCloseRequested();
    }
  };




  gridLayoutPanel->UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);
  contentPanel->UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);

  UseStyle(DUIDefaultStyles::FloatingWindowControlStyles::Default);

  resizeBox = CreateControl<ResizeDragControl>();

  resizeBox->OnResizeDrag = [this](DUIPoint delta) {
    size.width += delta.x;
    size.height += delta.y;

    size.width = clamp(size.width, 150, 3000);
    size.height = clamp(size.height, 150, 3000);

    MarkLayoutDirty();
  };
}

void FloatingWindowControl::DoLayout(Graphics* renderTarget) {
  (void)renderTarget;

  constexpr float titleHeight = 35.0f;

  gridLayoutPanel->SetLayoutFrame({ 0.0f, 0.0f }, size);

  resizeBox->SetLayoutFrame(
    {
      size.width - resizeBox->size.width / 2,
      size.height - resizeBox->size.height / 2
    },
    resizeBox->size
  );

  if (contentPanel != nullptr) {
    const DUISize contentSize {
      size.width,
      std::max(0.0f, size.height - titleHeight)
    };

    for (auto* child : contentPanel->Children) {
      if (child == nullptr)
        continue;

      child->SetLayoutFrame({ 1.0f, 1.0f }, contentSize - DUISize{2,2});
    }
  }
}

void FloatingWindowControl::OnRender(Graphics* rendTarget) {
  rendTarget->RenderBasicShadow(size);
  rendTarget->FillControlBackground(this);
}

void FloatingWindowControl::OnOverlayDismissed() {
  IOverlayControl::OnOverlayDismissed();

  if (OnOverlayDismissedHook != nullptr) {
    OnOverlayDismissedHook();
  }
}

void FloatingWindowControl::SetTitle(const std::string& title) {
  if (windowLabel != nullptr) {
    windowLabel->SetContent(title);
  }
}

void FloatingWindowControl::OnPointerDown(const PointerEvent &e) {
  if (OnRequestTopMost != nullptr) {
    OnRequestTopMost();
  }
}

void FloatingWindowControl::OnPointerUp(const PointerEvent &e) {

}

void FloatingWindowControl::OnMouseDrag(float deltaX, float deltaY) {

  SetLayoutLocation(location + DUIPoint{deltaX, deltaY});

}

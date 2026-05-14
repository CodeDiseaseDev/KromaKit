#include <kromakit/Checkbox.h>
#include <kromakit/styling/controls/CheckboxStyle.h>

Checkbox::Checkbox()
{
  UseStyle(DUIDefaultStyles::CheckboxStyles::Default);

  checkboxLabel = CreateControl<Label>();
  checkboxLabel->isHitTestVisible = false;

  // Control::AddControl(&checkboxLabel);

}

std::optional<std::string> Checkbox::GetControlName() const {
  return "Checkbox";
}

const std::vector<std::string> Checkbox::GetDebugOverlayItems() {
  return {};
}

void Checkbox::OnRender(
  Graphics* rendTarget)
{
  rendTarget->DrawEllipse(
    selected_circle_indic_rad, size.height / 2,
    selected_circle_indic_rad, ColorFromRGB(150, 150, 150));

  if (isChecked)
  {
    float rad = selected_circle_indic_rad - 4;

    auto col = ColorFromRGB(100, 100, 255);
    float shadowPx = 1,
          shadowOpacity = 80;

    // auto col = ColorFromRGB(0,0,0);
    // rendTarget->FillRadialGradient(
    //   selected_circle_indic_rad, size.height / 2,
    //   rad - shadowPx, rad + shadowPx,
    //   Alpha(col, (int)shadowOpacity), Alpha(col, 0));

    rendTarget->FillEllipse(
      selected_circle_indic_rad, size.height / 2,
      rad, ColorFromRGB(100, 100, 255));
  }

  // rendTarget->FillControlBackground(this);
}

bool Checkbox::WantsClickCapture() const {
  return true;
}

void Checkbox::DoLayout(Graphics* renderTarget)
{
  checkboxLabel->location =
  {
    selected_circle_indic_rad * 2 + spacing, 0
  };
  checkboxLabel->DoLayout(renderTarget);

  size = {
    checkboxLabel->location.x + checkboxLabel->size.width,
    std::max(
      selected_circle_indic_rad * 2.0f,
      checkboxLabel->size.height
    )
  };
}

void Checkbox::OnMouseDown()
{
  isChecked = !isChecked;

  if (OnCheckedChanged != nullptr)
  {
    OnCheckedChanged(isChecked);
  }
}

void Checkbox::SetText(
  std::string str)
{
  checkboxLabel->sContent = str;
}

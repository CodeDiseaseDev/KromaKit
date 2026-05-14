// #include <kromakit/ControlPreview.h>
//
// void ControlPreview::OnRender(
//   Graphics* graphics)
// {
//
//
//   graphics->FillRoundedRectangle(
//     0, 0, size.width, size.height,
//     background, 6
//   );
//
//   graphics->PushRectangleClip(
//     size.width, size.height);
//
//   if (TargetControl != nullptr)
//   {
//     TargetControl->RenderControl(graphics);
//
//   }
//
//   const float line_thickness = 3;
//   const float spacing = 20.0f;
//
//   float xStart = 0;
//   float yStart = 0;
//   float width = size.width;
//   float height = size.height;
//
//   for (float i = -height; i < width; i += spacing) {
//     float x1 = xStart + std::max(0.0f, i );
//     float y1 = yStart + std::max(0.0f, -i);
//
//     float x2 = xStart + std::min(size.width, i + size.height);
//     float y2 = yStart + std::min(size.height, size.height - (i + size.height - size.width));
//
//     graphics->DrawLine(
//       x1, y1, x2, y2,
//       danger_lines_color,
//       line_thickness);
//   }
//
//   graphics->DrawRectangle(
//     0, 0, size.width, size.height,
//     danger_lines_color);
//
//   graphics->PopRectangleClip();
// }
//
// void ControlPreview::DoLayout(Graphics* renderTarget)
// {
//   if (TargetControl != nullptr)
//   {
//     size = TargetControl->size;
//   }
// }

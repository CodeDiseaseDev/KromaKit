//
// Created by code on 4/29/26.
//

#include <kromakit/graphics/types/DUIRect.h>

#include <algorithm>

#include <kromakit/graphics/types/DUIInsets.h>
#include <kromakit/graphics/types/DUIPoint.h>
#include <kromakit/graphics/types/DUISize.h>

DUIRect DUIRect::operator+(const DUISize& s) const {
  return { x, y, width + s.width, height + s.height };
}

DUIRect::DUIRect() = default;

DUIRect::DUIRect(DUIPoint location, DUISize size)
    : x(location.x),
      y(location.y),
      width(size.width),
      height(size.height) {

}

DUIRect::DUIRect(float all) { x = y = width = height = all; }

DUIRect::DUIRect(float x, float y, float width, float height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

DUIRect DUIRect::operator+(const DUIPoint &p) const {
  return { x + p.x, y + p.y, width, height };
}

DUIRect DUIRect::operator-(const DUIPoint &p) const {
  return { x - p.x, y - p.y, width, height };
}

DUIRect & DUIRect::operator+=(const DUIPoint &p) {
  x += p.x; y += p.y;
  return *this;
}

DUIRect DUIRect::operator-(const DUISize &s) const {
  return { x, y, width - s.width, height - s.height };
}

DUIRect DUIRect::Inset(const DUIInsets &insets) const {
  return {
    x + insets.left,
    y + insets.top,
    width - insets.left - insets.right,
    height - insets.top - insets.bottom
  };
}

float DUIRect::Right() const { return x + width; }

float DUIRect::Bottom() const { return y + height; }

bool DUIRect::IsEmpty() const {
  return width <= 0 || height <= 0;
}

bool DUIRect::Contains(const DUIPoint &p) const {
  return p.x >= x && p.x <= x + width &&
         p.y >= y && p.y <= y + height;
}

bool DUIRect::ContainsRect(const DUIRect &rect) const {
  return
      Contains({rect.x, rect.y}) &&
      Contains({rect.x + rect.width, rect.y + rect.height});
}

bool DUIRect::Intersects(const DUIRect &other) const {
  return !(other.x > x + width ||
           other.x + other.width < x ||
           other.y > y + height ||
           other.y + other.height < y);
}

DUIRect DUIRect::IntersectRects(const DUIRect &b) {
  float left   = std::max(x, b.x);
  float top    = std::max(y, b.y);
  float right  = std::min(Right(), b.Right());
  float bottom = std::min(Bottom(), b.Bottom());

  float w = right - left;
  float h = bottom - top;

  // no overlap → return empty rect
  if (w <= 0 || h <= 0)
    return { 0, 0, 0, 0 };

  return { left, top, w, h };
}

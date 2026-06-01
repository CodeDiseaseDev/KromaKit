//
// Created by code on 4/29/26.
//

#include <kromakit/graphics/types/DUIPoint.h>
#include <cmath>

static float SnapFloatToPixel(float value, float scaleFactor) {
  if (scaleFactor <= 0.0f) {
    return std::round(value);
  }

  return std::round(value * scaleFactor) / scaleFactor;
}

DUIPoint::DUIPoint(float _x, float _y) :
  x(_x), y(_y) {}

DUIPoint::DUIPoint() {
  x = y = 0;
}

DUIPoint DUIPoint::operator+(const DUIPoint &other) const {
  return { x + other.x, y + other.y };
}

DUIPoint DUIPoint::operator-(const DUIPoint &other) const {
  return { x - other.x, y - other.y };
}

DUIPoint DUIPoint::operator*(float scalar) const {
  return { x * scalar, y * scalar };
}

DUIPoint DUIPoint::operator/(float scalar) const {
  return { x / scalar, y / scalar };
}

DUIPoint & DUIPoint::operator+=(const DUIPoint &other) {
  x += other.x; y += other.y;
  return *this;
}

DUIPoint & DUIPoint::operator-=(const DUIPoint &other) {
  x -= other.x; y -= other.y;
  return *this;
}

bool DUIPoint::operator==(const DUIPoint &other) const {
  return x == other.x && y == other.y;
}

bool DUIPoint::operator!=(const DUIPoint &other) const {
  return !(*this == other);
}

DUIPoint DUIPoint::SnapToPixel(float scaleFactor) const {
  return {
    SnapFloatToPixel(x, scaleFactor),
    SnapFloatToPixel(y, scaleFactor)
  };
}

void DUIPoint::SnapToPixelInPlace(float scaleFactor) {
  x = SnapFloatToPixel(x, scaleFactor);
  y = SnapFloatToPixel(y, scaleFactor);
}
//
// Created by code on 4/29/26.
//

#include <kromakit/graphics/types/DUISize.h>

DUISize DUISize::operator+(const DUISize &other) const {
  return { width + other.width, height + other.height };
}

DUISize DUISize::operator-(const DUISize &other) const {
  return { width - other.width, height - other.height };
}

DUISize DUISize::operator*(float scalar) const {
  return { width * scalar, height * scalar };
}

DUISize DUISize::operator/(float scalar) const {
  return { width / scalar, height / scalar };
}

bool DUISize::operator==(const DUISize &other) const {
  return width == other.width && height == other.height;
}

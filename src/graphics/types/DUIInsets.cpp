//
// Created by code on 4/29/26.
//

#include <kromakit/graphics/types/DUIInsets.h>

#include <kromakit/graphics/types/DUIRect.h>
#include <kromakit/graphics/types/DUISize.h>

DUIInsets::DUIInsets() :
  left(0), top(0),
  right(0), bottom(0) {}

DUIInsets::DUIInsets(float all):
  left(all), top(all),
  right(all), bottom(all) {}

DUIInsets::DUIInsets(
  float horizontal, float vertical):
    left(horizontal), top(vertical),
    right(horizontal), bottom(vertical) {}

DUIInsets::DUIInsets(
  float left, float top,
  float right, float bottom):
    left(left), top(top),
    right(right), bottom(bottom) {}

DUIInsets DUIInsets::Zero() {
  return DUIInsets(0);
}

float DUIInsets::Horizontal() const {
  return left + right;
}

float DUIInsets::Vertical() const {
  return top + bottom;
}

bool DUIInsets::IsZero() const {
  return left == 0 && top == 0 && right == 0 && bottom == 0;
}

bool DUIInsets::IsEmpty() const {
  return IsZero();
}

DUIInsets DUIInsets::operator+(const DUIInsets &other) const {
  return {
    left + other.left,
    top + other.top,
    right + other.right,
    bottom + other.bottom
  };
}

DUIInsets DUIInsets::operator-(const DUIInsets &other) const {
  return {
    left - other.left,
    top - other.top,
    right - other.right,
    bottom - other.bottom
  };
}

DUIInsets & DUIInsets::operator+=(const DUIInsets &other) {
  left += other.left;
  top += other.top;
  right += other.right;
  bottom += other.bottom;
  return *this;
}

DUIInsets & DUIInsets::operator-=(const DUIInsets &other) {
  left -= other.left;
  top -= other.top;
  right -= other.right;
  bottom -= other.bottom;
  return *this;
}

bool DUIInsets::operator==(const DUIInsets &other) const {
  return left == other.left &&
         top == other.top &&
         right == other.right &&
         bottom == other.bottom;
}

bool DUIInsets::operator!=(const DUIInsets &other) const {
  return !(*this == other);
}

DUISize DUIInsets::GetAddedSize() const {
  return {
    Horizontal(),
    Vertical()
  };
}

DUIRect DUIInsets::InflateRect(const DUIRect &rect) const {
  return {
    rect.x - left,
    rect.y - top,
    rect.width + left + right,
    rect.height + top + bottom
  };
}

DUIRect DUIInsets::DeflateRect(const DUIRect &rect) const {
  return {
    rect.x + left,
    rect.y + top,
    rect.width - left - right,
    rect.height - top - bottom
  };
}

DUIInsets DUIInsets::LeftInset(float x) {
  return DUIInsets(x, 0, 0, 0);
}

DUIInsets DUIInsets::TopInset(float x) {
  return DUIInsets(0, x, 0, 0);
}

DUIInsets DUIInsets::RightInset(float x) {
  return DUIInsets(0, 0, x, 0);
}

DUIInsets DUIInsets::BottomInset(float x) {
  return DUIInsets(0, 0, 0, x);
}

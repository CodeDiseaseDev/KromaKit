//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_DUIPOINT_H
#define TEST_IOS_APP_DUIPOINT_H

struct DUIRect;
struct DUIInsets;
struct DUIRect;
struct DUISize;

struct DUIPoint {
  float x, y;

  DUIPoint(float _x, float _y);
  DUIPoint();

  // Addition / subtraction
  DUIPoint operator+(const DUIPoint& other) const;
  DUIPoint operator-(const DUIPoint& other) const;

  // Scalar multiply / divide
  DUIPoint operator*(float scalar) const;
  DUIPoint operator/(float scalar) const;

  // Compound
  DUIPoint& operator+=(const DUIPoint& other);
  DUIPoint& operator-=(const DUIPoint& other);

  // Equality
  bool operator==(const DUIPoint& other) const;
  bool operator!=(const DUIPoint& other) const;

  DUIPoint SnapToPixel(float scaleFactor = 1.0f) const;
  void SnapToPixelInPlace(float scaleFactor = 1.0f);
};



#endif //TEST_IOS_APP_DUIPOINT_H

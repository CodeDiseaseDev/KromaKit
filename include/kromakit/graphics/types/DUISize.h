//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_DUISIZE_H
#define TEST_IOS_APP_DUISIZE_H



struct DUISize {
  float width, height;

  DUISize operator+(const DUISize& other) const;
  DUISize operator-(const DUISize& other) const;
  DUISize operator*(float scalar) const;
  DUISize operator/(float scalar) const;

  bool operator==(const DUISize& other) const;
};


#endif //TEST_IOS_APP_DUISIZE_H

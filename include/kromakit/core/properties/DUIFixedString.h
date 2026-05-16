#ifndef TEST_IOS_APP_DUIFIXEDSTRING_H
#define TEST_IOS_APP_DUIFIXEDSTRING_H

#include <cstddef>
#include <string_view>

template <std::size_t N>
struct DUIFixedString {
  char value[N];

  constexpr DUIFixedString(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; i++)
      value[i] = str[i];
  }

  constexpr operator std::string_view() const {
    return std::string_view(value, N - 1);
  }
};

#endif // TEST_IOS_APP_DUIFIXEDSTRING_H
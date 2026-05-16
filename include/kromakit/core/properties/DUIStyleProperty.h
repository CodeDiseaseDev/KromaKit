#ifndef TEST_IOS_APP_DUISTYLEPROPERTY_H
#define TEST_IOS_APP_DUISTYLEPROPERTY_H

#include <kromakit/core/properties/DUIProperty.h>

template <typename T, DUIFixedString Name>
using DUIStyleProperty = DUIProperty<T, Name, DUIPropertyKind::Style>;

#endif // TEST_IOS_APP_DUISTYLEPROPERTY_H
#ifndef TEST_IOS_APP_DUIVISUALPROPERTY_H
#define TEST_IOS_APP_DUIVISUALPROPERTY_H

#include <kromakit/core/properties/DUIProperty.h>

template <typename T, DUIFixedString Name>
using DUIVisualProperty = DUIProperty<T, Name, DUIPropertyKind::Visual>;

#endif // TEST_IOS_APP_DUIVISUALPROPERTY_H
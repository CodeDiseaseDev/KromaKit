#ifndef TEST_IOS_APP_DUITRANSFORMPROPERTY_H
#define TEST_IOS_APP_DUITRANSFORMPROPERTY_H

#include <kromakit/core/properties/DUIProperty.h>

template <typename T, DUIFixedString Name>
using DUITransformProperty = DUIProperty<T, Name, DUIPropertyKind::Transform>;

#endif // TEST_IOS_APP_DUITRANSFORMPROPERTY_H
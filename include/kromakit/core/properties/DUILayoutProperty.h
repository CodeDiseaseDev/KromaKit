#ifndef TEST_IOS_APP_DUILAYOUTPROPERTY_H
#define TEST_IOS_APP_DUILAYOUTPROPERTY_H

#include <kromakit/core/properties/DUIProperty.h>

template <typename T, DUIFixedString Name>
using DUILayoutProperty = DUIProperty<T, Name, DUIPropertyKind::Layout>;

#endif // TEST_IOS_APP_DUILAYOUTPROPERTY_H

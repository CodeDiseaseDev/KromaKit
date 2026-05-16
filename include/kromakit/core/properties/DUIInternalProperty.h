#ifndef TEST_IOS_APP_DUIINTERNALPROPERTY_H
#define TEST_IOS_APP_DUIINTERNALPROPERTY_H

#include <kromakit/core/properties/DUIProperty.h>

template <typename T, DUIFixedString Name>
using DUIInternalProperty = DUIProperty<T, Name, DUIPropertyKind::Internal>;

#endif // TEST_IOS_APP_DUIINTERNALPROPERTY_H

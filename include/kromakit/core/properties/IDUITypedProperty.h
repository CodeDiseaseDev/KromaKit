#ifndef TEST_IOS_APP_IDUITYPEDPROPERTY_H
#define TEST_IOS_APP_IDUITYPEDPROPERTY_H

#include <kromakit/core/properties/IDUIProperty.h>

template <typename T>
class IDUITypedProperty : public IDUIProperty {
public:
  virtual const T& Get() const = 0;
  virtual void Set(const T& newValue) = 0;
};

#endif // TEST_IOS_APP_IDUITYPEDPROPERTY_H
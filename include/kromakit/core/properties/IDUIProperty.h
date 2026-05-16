#ifndef TEST_IOS_APP_IDUIPROPERTY_H
#define TEST_IOS_APP_IDUIPROPERTY_H

#include <string_view>
#include <typeindex>

#include <kromakit/core/properties/DUIPropertyKind.h>

class IDUIProperty {
public:
  virtual ~IDUIProperty() = default;

  IDUIProperty(const IDUIProperty&) = delete;
  IDUIProperty& operator=(const IDUIProperty&) = delete;

  IDUIProperty(IDUIProperty&&) = delete;
  IDUIProperty& operator=(IDUIProperty&&) = delete;

  virtual std::string_view GetName() const = 0;
  virtual std::type_index GetValueType() const = 0;
  virtual DUIPropertyKind GetKind() const = 0;

protected:
  IDUIProperty() = default;
};

#endif // TEST_IOS_APP_IDUIPROPERTY_H
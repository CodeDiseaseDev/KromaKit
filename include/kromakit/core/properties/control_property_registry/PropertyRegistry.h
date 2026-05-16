#ifndef TEST_IOS_APP_PROPERTYREGISTRY_H
#define TEST_IOS_APP_PROPERTYREGISTRY_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <kromakit/core/properties/IDUIProperty.h>
#include <kromakit/core/properties/IDUITypedProperty.h>
#include <kromakit/core/properties/DUIProperty.h>
#include <kromakit/core/properties/DUIInternalProperty.h>
#include <kromakit/core/properties/DUIVisualProperty.h>

class PropertyRegistry {
public:
  explicit PropertyRegistry(IPropertyListener* listener)
    : listener(listener) {
  }

  void Register(IDUIProperty* property);

  IDUIProperty* Find(std::string_view name) const;

  template <typename T>
  IDUITypedProperty<T>* FindTyped(std::string_view name) const {
    auto* property = Find(name);

    if (!property)
      return nullptr;

    return dynamic_cast<IDUITypedProperty<T>*>(property);
  }

  template <typename T, DUIFixedString Name, DUIPropertyKind Kind = DUIPropertyKind::Unknown>
  DUIProperty<T, Name, Kind>* Find() const {
    auto it = byName.find(std::string(std::string_view(Name)));

    if (it == byName.end())
      return nullptr;

    return dynamic_cast<DUIProperty<T, Name, Kind>*>(it->second);
  }

  const std::vector<IDUIProperty*>& GetProperties() const;

  template <typename T, DUIFixedString Name, DUIPropertyKind Kind>
  DUIProperty<T, Name, Kind> NewProperty(const T& defaultValue = {}) {
    return DUIProperty<T, Name, Kind>(defaultValue, listener);
  }

  template <typename T, DUIFixedString Name>
  DUIVisualProperty<T, Name> NewVisualProperty(const T& defaultValue = {}) {
    return NewProperty<T, Name, DUIPropertyKind::Visual>(defaultValue);
  }

  template <typename T, DUIFixedString Name>
  DUIInternalProperty<T, Name> NewInternalProperty(const T& defaultValue = {}) {
    return NewProperty<T, Name, DUIPropertyKind::Internal>(defaultValue);
  }

private:
  IPropertyListener* listener = nullptr;

  std::vector<IDUIProperty*> properties;
  std::unordered_map<std::string, IDUIProperty*> byName;


};

#endif // TEST_IOS_APP_PROPERTYREGISTRY_H

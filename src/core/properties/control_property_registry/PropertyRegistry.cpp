#include <kromakit/core/properties/control_property_registry/PropertyRegistry.h>

void PropertyRegistry::Register(IDUIProperty* property) {
  if (!property)
    return;

  std::string name(property->GetName());

  properties.push_back(property);
  byName[name] = property;
}

IDUIProperty* PropertyRegistry::Find(std::string_view name) const {
  auto it = byName.find(std::string(name));

  if (it == byName.end())
    return nullptr;

  return it->second;
}

const std::vector<IDUIProperty *> & PropertyRegistry::GetProperties() const {
  return properties;
}

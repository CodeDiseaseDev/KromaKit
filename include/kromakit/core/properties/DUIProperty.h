#ifndef TEST_IOS_APP_DUIPROPERTY_H
#define TEST_IOS_APP_DUIPROPERTY_H

#include <typeindex>
#include <utility>

#include <kromakit/core/properties/DUIFixedString.h>
#include <kromakit/core/properties/IDUITypedProperty.h>
#include <kromakit/Utils.h>
#include <kromakit/core/properties/listener/IPropertyListener.h>

template <typename T, DUIFixedString Name, DUIPropertyKind Kind = DUIPropertyKind::Unknown>
class DUIProperty : public IDUITypedProperty<T> {
public:
  DUIProperty() = default;

  explicit DUIProperty(const T& defaultValue)
    : value(defaultValue) {
  }

  DUIProperty(const T& defaultValue, IPropertyListener* listener)
    : value(defaultValue), listener(listener) {
  }

  DUIProperty& operator=(const T& newValue) {
    Set(newValue);
    return *this;
  }

  const T& Get() const override {
    return value;
  }

  void Set(const T& newValue) override {
    if (value != newValue) {
      value = newValue;
      NotifyChanged();
    }
  }

  operator const T&() const {
    return value;
  }

  /// Returns a mutable reference to the stored value without notifying listeners.
  ///
  /// Mutating the returned reference will bypass `Set()`, meaning property
  /// listeners will not receive `OnPropertyChanged()`.
  ///
  /// Prefer `Set()`, assignment, compound operators, or `Modify()` unless you
  /// intentionally need to bypass notification.
  T& UnsafeGetMutable() {
    return value;
  }

  template <typename Func>
  void Modify(Func&& func) {
    T oldValue = value;

    func(value);

    if (value != oldValue)
      NotifyChanged();
  }

  std::string_view GetName() const override {
    return Name;
  }

  std::type_index GetValueType() const override {
    return typeid(T);
  }

  DUIPropertyKind GetKind() const override {
    return Kind;
  }

  void SetListener(IPropertyListener* newListener) {
    listener = newListener;
  }

  template <typename U>
  DUIProperty& operator+=(const U& rhs) {
    Set(value + rhs);
    return *this;
  }

  template <typename U>
  DUIProperty& operator-=(const U& rhs) {
    Set(value - rhs);
    return *this;
  }

  template <typename U>
  DUIProperty& operator*=(const U& rhs) {
    Set(value * rhs);
    return *this;
  }

  template <typename U>
  DUIProperty& operator/=(const U& rhs) {
    Set(value / rhs);
    return *this;
  }

  DUIProperty& operator++() {
    Set(value + 1);
    return *this;
  }

  T operator++(int) {
    T oldValue = value;
    Set(value + 1);
    return oldValue;
  }

  DUIProperty& operator--() {
    Set(value - 1);
    return *this;
  }

  T operator--(int) {
    T oldValue = value;
    Set(value - 1);
    return oldValue;
  }

  const T* operator->() const {
    return &value;
  }

  const T& operator*() const {
    return value;
  }

private:
  void NotifyChanged() {
    if (listener)
      listener->OnPropertyChanged(this);
  }

private:
  T value {};
  IPropertyListener* listener = nullptr;
};

#endif // TEST_IOS_APP_DUIPROPERTY_H
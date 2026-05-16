#ifndef TEST_IOS_APP_IPROPERTYLISTENER_H
#define TEST_IOS_APP_IPROPERTYLISTENER_H

class IDUIProperty;

class IPropertyListener {
public:
  virtual ~IPropertyListener() = default;

  virtual void OnPropertyChanged(IDUIProperty* property) = 0;
};

#endif // TEST_IOS_APP_IPROPERTYLISTENER_H
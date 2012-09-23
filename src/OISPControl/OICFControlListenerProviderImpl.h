#ifndef OICF_CONTROL_LISTENER_PROVIDER_IMPL_H
#define OICF_CONTROL_LISTENER_PROVIDER_IMPL_H

#include <dbus-c++/dbus.h>
#include <OICFControl/OICFControlListenerProvider.h>
#include "Joystick.h"

class OICFControlListenerProviderImpl : public OICFControlListenerProvider
{
public:

  OICFControlListenerProviderImpl(DBus::Connection &connection);

  std::map< std::string, std::string > Info();
protected:
  void onAxisListenerWrap(const EventJoystick &event);
  void onButtonListenerWrap(const EventJoystick &event);

private:
  Joystick joystick;
};

#endif// OICF_CONTROL_LISTENER_PROVIDER_IMPL_H

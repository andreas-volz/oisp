#ifndef OICF_CONTROL_LISTENER_PROVIDER_IMPL_H
#define OICF_CONTROL_LISTENER_PROVIDER_IMPL_H

#include <dbus-c++/dbus.h>
#include <OICFControl/OICFControlListenerProvider.h>

class OICFControlListenerProviderImpl : public OICFControlListenerProvider
{
public:

  OICFControlListenerProviderImpl(DBus::Connection &connection);

  std::map< std::string, std::string > Info();

protected:

private:


};

#endif // OICF_CONTROL_LISTENER_PROVIDER_IMPL_H

#ifndef OICF_NAVIGATION_PROVIDERIMPL_H
#define OICF_NAVIGATION_PROVIDERIMPL_H

#include <dbus-c++/dbus.h>
#include <OICFNavigation/OICFNavigationProvider.h>
#include "../common/Logger.h"

/* forward declarations */
class Navigation;

class OICFNavigationProviderImpl : public OICFNavigationProvider
{
public:
  OICFNavigationProviderImpl(DBus::Connection &connection);

  std::map< std::string, std::string > Info();

  void setNavigation(Navigation *navigation);

private:
  void moveMapSteps(const int32_t &direction, const int32_t &steps);
  void zoomIn();
  void zoomOut();
  void jumpToPointer();

  void searchCity(const std::string& city);
  void getNextValidCharacters();

  // those generic calls are created to test new features fast without adding new functions to oicf
  void generic1(const std::string &param1, const std::string &param2, const std::string &param3);
  void generic2(const std::string &param1, const std::string &param2, const std::string &param3);
  void generic3(const std::string &param1, const std::string &param2, const std::string &param3);

  Logger mLogger;
  Navigation *m_navigation;
};

#endif // OICF_NAVIGATION_PROVIDER_IMPL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

/* libosmscout */
#include <osmscout/Database.h>
#include <osmscout/MapPainter.h>
#include <osmscout/StyleConfigLoader.h>

/* STD */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <iostream>

/* DBus-C++ */
#include <dbus-c++/dbus.h>
#include <dbus-c++/ecore-integration.h>

/* local */
#include "ScreenManager.h"
#include "NavigationScreen.h"
#include "Navigation.h"
#include "OICFNavigationProviderImpl.h"

static const char *MAP_VIEWER_NAME = "org.oicf.Navigation";

using namespace std;
using namespace Eflxx;

static const Eflxx::Size initialWindowSize(800, 480);

DBus::Ecore::BusDispatcher dispatcher;

void quit()
{
  // TODO: call ScreenManager quit() or so...
  Ecorexx::Application::quit();
}

void niam(int sig)
{
  quit();
}

Eina_Bool eina_list_init(void);

int main(int argc, char **argv)
{
  signal(SIGTERM, niam);
  signal(SIGINT, niam);

  // create and init ScreenManager (and Ecore!!)
  ScreenManager &screenManager(ScreenManager::instance());
  screenManager.init(argc, argv, initialWindowSize);

  // initialize Glib thread system
  if (!Glib::thread_supported()) Glib::thread_init();

  //DBus::_init_threading(); ??

  // DBus-C++ stuff first
  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();
  conn.request_name(MAP_VIEWER_NAME);

  // send signals to GUI with this...
  OICFNavigationListenerProvider mapViewerListenerProvider(conn);

  // get signals from GUI through this...
  OICFNavigationProviderImpl mapProvider(conn);

  // create screen after ScreenManager initialization!
  NavigationScreen navigationScreen;

  // create navigation object after DBus and screen init
  Navigation navigation (&mapViewerListenerProvider);

  navigationScreen.setNavigation(&navigation);
  mapProvider.setNavigation(&navigation);

  navigation.setNavigationScreen(&navigationScreen);

  navigation.setOICFNavigationListenerProvider(&mapViewerListenerProvider);

  navigation.start();

  cout << "OISPNavigation server started..." << endl;

  screenManager.app->exec();

  return 0;
}

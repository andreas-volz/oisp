#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <signal.h>
#include "OICFControlListenerProviderImpl.h"
#include <dbus-c++/dbus.h>

using namespace std;

static const char *ECHO_SERVER_NAME = "org.oicf.Control";

DBus::BusDispatcher dispatcher;

void niam(int sig)
{
  dispatcher.leave();
}

int main()
{
  signal(SIGTERM, niam);
  signal(SIGINT, niam);

  // initialize Glib thread system
  if (!Glib::thread_supported()) Glib::thread_init();

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  conn.request_name(ECHO_SERVER_NAME);

  OICFControlListenerProviderImpl server(conn);

  cout << "OICPControl server started..." << endl;

  dispatcher.enter();

  return 0;
}

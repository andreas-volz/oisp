#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <math.h>
#include <iostream>
#include <signal.h>
#include "OICFMediaProviderImpl.h"

/* EFLxx */
#include <ecorexx/Ecorexx.h>
#include <emotionxx/Emotionxx.h>

#include "Player.h"

#include <dbus-c++/dbus.h>
#include <dbus-c++/ecore-integration.h>

//#include <glib/glib.h>
//#include <glib/gthread.h>

static const char *SERVER_NAME = "org.oicf.Media";

using namespace std;
using namespace Eflxx;

int width = 800;
int height = 600;

DBus::Ecore::BusDispatcher dispatcher;

void niam(int sig)
{
  //dispatcher.leave();
  //app->quit ();
  cerr << "Quit application..." << endl;
}

int main(int argc, const char **argv)
{
  // initialize DBus thread system
  DBus::_init_threading();

  signal(SIGTERM, niam);
  signal(SIGINT, niam);

  /* Create the application object */
  Ecorexx::Application app(argc, argv, "OISPMedia");

  /* Create the main window, a window with an embedded canvas */
  Ecorexx::EvasWindowSoftwareX11 *mw = new Ecorexx::EvasWindowSoftwareX11(Size(0, 0));
  Evasxx::Canvas &evas = mw->getCanvas();

  /* Create Emotionxx::Object object using xine engine */
  Emotionxx::AudioObject *emotion = new Emotionxx::AudioObject(evas, "gstreamer");

  Player player(emotion);

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  conn.request_name(SERVER_NAME);

  OICFMediaListenerProvider mediaListenerProvider(conn);

  OICFMediaProviderImpl server(conn);

  server.setOICFMediaListenerProvider(&mediaListenerProvider);
  server.setPlayer(&player);

  // needed to intialize the first startup list
  server.getWindowList(0, 100);

  cout << "OISPMedia server started..." << endl;

  /* Enter the application main loop */
  app.exec();

  return 0;
}

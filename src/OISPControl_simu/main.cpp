#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <memory>

/* DBUS */
#include <dbus-c++/dbus.h>
#include <dbus-c++/ecore-integration.h>

/* EFL */
#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>

/* local */
#include "searchFile.h"
#include "OICFControlListenerProviderImpl.h"

#define WIDTH 400
#define HEIGHT 400

static const char *ECHO_SERVER_NAME = "org.oicf.ControlListener";
DBus::Ecore::BusDispatcher dispatcher;
OICFControlListenerProviderImpl *controlServer;

using namespace Eflxx;
using namespace std;

void buttonFunc(const std::string emmision, const std::string source)
{
  cout << "buttonFunc" << endl;

  KeyEvent keyEvent;
  keyEvent.time = 0; // FIXME

  keyEvent.number = KeyEvent::Media;
  keyEvent.value = KeyEvent::Down;
  controlServer->onButtonListener(keyEvent);
}

void evas_quit(const Ecorexx::EvasWindow &win)
{
  Ecorexx::Application::quit();
}

int main( int argc, const char **argv )
{
  // Create the application object
  Ecorexx::Application app (argc, argv, "OISPControl Panel Simulator");

  DBus::default_dispatcher = &dispatcher;

  ///dispatcher.attach(NULL);

  DBus::Connection conn = DBus::Connection::SessionBus();

  conn.request_name(ECHO_SERVER_NAME);

  controlServer = new OICFControlListenerProviderImpl(conn);

  cout << "OICPControl_keyb server started..." << endl;
  
  Size s (WIDTH, HEIGHT);

  // Create the main window, a window with an embedded canvas
  Ecorexx::EvasWindowSoftwareX11 mw (s);
  mw.deleteRequestSignal.connect(sigc::ptr_fun(&evas_quit));
  
  Evasxx::Canvas &evas = mw.getCanvas();
  
  // Add some objects to the canvas
  Edjexx::Object edje (evas, Point (0, 0), searchDataFile ("OISPControl_simu/themes/panel/OISPControl_simu_panel.edj"), "OISPControl_simu");
  
  edje.connect("clicked", "button_media", sigc::ptr_fun(&buttonFunc));
  
  edje.resize( s );

  edje.setLayer( 0 );
  edje.show();
  
  mw.show();

  // Enter the application main loop
  app.exec();

  delete controlServer;

  cout << "OICPControl_keyb server stopped..." << endl;

  return 0;
}


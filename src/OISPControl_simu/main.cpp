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

void quit()
{
  Ecorexx::Application::quit();
}


void evas_quit(const Ecorexx::EvasWindow &win)
{
  quit();  
}

void buttonFunc(const std::string emmision, const std::string source)
{
  cout << "buttonFunc" << endl;

  KeyEvent keyEvent;
  keyEvent.time = 0; // FIXME

  keyEvent.number = KeyEvent::Media;
  keyEvent.value = KeyEvent::Down;
  controlServer->onButtonListener(keyEvent);
}

void keyDownHandler (const Evasxx::KeyDownEvent &key)
{
  printf("You hit key: %s\n", key.data->keyname);

  KeyEvent keyEvent;
  keyEvent.time = key.data->timestamp;
  
  if(string(key.data->keyname) == "Left")
  {
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Min;
    controlServer->onAxisListener(keyEvent);
  }
  else if(string(key.data->keyname) == "Up")
  {
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Max;
    controlServer->onAxisListener(keyEvent);
  }
  else if(string(key.data->keyname) == "Right")
  {
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Max;
    controlServer->onAxisListener(keyEvent);
  }
  else if(string(key.data->keyname) == "Down")
  {
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Min;
    controlServer->onAxisListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F1")
  {
    keyEvent.number = KeyEvent::Navigation;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F2")
  {
    keyEvent.number = KeyEvent::Media;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F3")
  {
    keyEvent.number = KeyEvent::Test;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F4")
  {
    keyEvent.number = KeyEvent::Test2;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F5")
  {
    keyEvent.number = KeyEvent::One;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F6")
  {
    keyEvent.number = KeyEvent::Two;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F7")
  {
    keyEvent.number = KeyEvent::Three;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F8")
  {
    keyEvent.number = KeyEvent::Four;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F9")
  {
    keyEvent.number = KeyEvent::Start;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "F10")
  {
    keyEvent.number = KeyEvent::Menu;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
  }
  else if(string(key.data->keyname) == "q")
  {
    quit();
  }
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

  edje.signalHandleKeyDown.connect(sigc::ptr_fun(&keyDownHandler));
  
  edje.resize( s );

  edje.setFocus(true);
  edje.show();

  // TODO:
  // set OILM layer: Utility
  // set optional "stay on top" hint
  
  mw.show();

  // Enter the application main loop
  app.exec();

  delete controlServer;

  cout << "OICPControl_keyb server stopped..." << endl;

  return 0;
}


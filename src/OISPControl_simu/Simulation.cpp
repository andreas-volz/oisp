#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <memory>

/* DBUS */
#include <dbus-c++/dbus.h>
#include <dbus-c++/ecore-integration.h>

/* local */
#include "searchFile.h"
#include "OICFControlListenerProviderImpl.h"
#include "Simulation.h"

#define WIDTH 400
#define HEIGHT 400

using namespace Eflxx;
using namespace std;

static const char *ECHO_SERVER_NAME = "org.oicf.ControlListener";
DBus::Ecore::BusDispatcher dispatcher;
OICFControlListenerProviderImpl *controlServer;

Simulation::Simulation()
{
}

int Simulation::exec(int argc, const char **argv)
{
  // Create the application object
  Ecorexx::Application app (argc, argv, "OISPControl Panel Simulator");

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  conn.request_name(ECHO_SERVER_NAME);

  controlServer = new OICFControlListenerProviderImpl(conn);

  cout << "OICPControl_simu server started..." << endl;
  
  Size s (WIDTH, HEIGHT);

  // Create the main window, a window with an embedded canvas
  Ecorexx::EvasWindowSoftwareX11 mw (s);
  mw.deleteRequestSignal.connect(sigc::mem_fun(this, &Simulation::evasQuit));
  
  Evasxx::Canvas &evas = mw.getCanvas();
  
  // Add some objects to the canvas
  edje = new Edjexx::Object(evas, Point (0, 0), searchDataFile ("OISPControl_simu/themes/panel/OISPControl_simu_panel.edj"), "OISPControl_simu");

  /* keys */
  edje->signalHandleKeyDown.connect(sigc::mem_fun(this, &Simulation::keyDownHandler));
  edje->signalHandleKeyUp.connect(sigc::mem_fun(this, &Simulation::keyUpHandler));

  /* DDS */
  edje->connect("tick", "dds_left", sigc::bind(sigc::mem_fun(this, &Simulation::rotaryHandler), 
                                                  RotaryEvent::DDS, RotaryEvent::Left));
  edje->connect("tick", "dds_right", sigc::bind(sigc::mem_fun(this, &Simulation::rotaryHandler), 
                                                  RotaryEvent::DDS, RotaryEvent::Right));  

  edje->connect("down", "dds_button", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                  ButtonEvent::DDS, ButtonEvent::Down));
  edje->connect("up", "dds_button", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                ButtonEvent::DDS, ButtonEvent::Up));
  /* Volume */
  edje->connect("tick", "volume_left", sigc::bind(sigc::mem_fun(this, &Simulation::rotaryHandler), 
                                                  RotaryEvent::Volume, RotaryEvent::Left));
  edje->connect("tick", "volume_right", sigc::bind(sigc::mem_fun(this, &Simulation::rotaryHandler), 
                                                  RotaryEvent::Volume, RotaryEvent::Right));  

  edje->connect("down", "volume_button", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                  ButtonEvent::Volume, ButtonEvent::Down));
  edje->connect("up", "volume_button", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                ButtonEvent::Volume, ButtonEvent::Up));
  /* Menu */
  edje->connect("down", "button_menu", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                  ButtonEvent::Menu, ButtonEvent::Down));
  edje->connect("up", "button_menu", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                ButtonEvent::Menu, ButtonEvent::Up));
  /* Navigation */
  edje->connect("down", "button_nav", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                 ButtonEvent::Navigation, ButtonEvent::Down));
  edje->connect("up", "button_nav", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                               ButtonEvent::Navigation, ButtonEvent::Up));
  /* Media */
  edje->connect("down", "button_media", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                   ButtonEvent::Media, ButtonEvent::Down));
  edje->connect("up", "button_media", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                 ButtonEvent::Media, ButtonEvent::Up));
  /* Phone */
  edje->connect("down", "button_phone", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                   ButtonEvent::Phone, ButtonEvent::Down));
  edje->connect("up", "button_phone", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                 ButtonEvent::Phone, ButtonEvent::Up));
  /* Return */
  edje->connect("down", "button_return", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                    ButtonEvent::Return, ButtonEvent::Down));
  edje->connect("up", "button_return", sigc::bind(sigc::mem_fun(this, &Simulation::buttonHandler), 
                                                  ButtonEvent::Return, ButtonEvent::Up));
  /* X Axis */
  edje->connect("min", "axis_x", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                    AxisEvent::X, AxisEvent::Min));
  edje->connect("max", "axis_x", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                  AxisEvent::X, AxisEvent::Max));
  edje->connect("zero", "axis_x", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                  AxisEvent::X, AxisEvent::Zero));
  /* Y Axis */
  edje->connect("min", "axis_y", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                    AxisEvent::Y, AxisEvent::Min));
  edje->connect("max", "axis_y", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                  AxisEvent::Y, AxisEvent::Max));
  edje->connect("zero", "axis_y", sigc::bind(sigc::mem_fun(this, &Simulation::axisHandler), 
                                                  AxisEvent::Y, AxisEvent::Zero));
      
  edje->resize( s );

  edje->setFocus(true);
  edje->show();

  // TODO:
  // set OILM layer: Utility
  // set optional "stay on top" hint
  
  mw.show();

  // Enter the application main loop
  app.exec();

  delete controlServer;
  delete edje;

  cout << "OICPControl_simu server stopped..." << endl;

  return 0;
}

void Simulation::quit()
{
  Ecorexx::Application::quit();
}

void Simulation::evasQuit(const Ecorexx::EvasWindow &win)
{
  quit();  
}

void Simulation::keyDownHandler (const Evasxx::KeyDownEvent &key)
{
  keyEdjeCaller(key.data->keyname, "mouse,down,1");
}

void Simulation::keyUpHandler (const Evasxx::KeyUpEvent &key)
{
  keyEdjeCaller(key.data->keyname, "mouse,up,1");
}

void Simulation::keyEdjeCaller (const std::string &key, const std::string &signal)
{
  cout << "key: " << key << endl;
  
  if(key == "Left")
  {
    edje->emit(signal, "cross_left_press_mask");
  }
  else if(key == "Up")
  {
    edje->emit(signal, "cross_up_press_mask");
  }
  else if(key == "Right")
  {
    edje->emit(signal, "cross_right_press_mask");
  }
  else if(key == "Down")
  {
    edje->emit(signal, "cross_down_press_mask");
  }
  else if(key == "Insert")
  {
    edje->emit(signal, "volume_left");
  }
  else if(key == "Home")
  {
    edje->emit(signal, "volume_button");
  }
  else if(key == "Prior")
  {
    edje->emit(signal, "volume_right");
  }
  else if(key == "Delete")
  {
    edje->emit(signal, "dds_left");
  }
  else if(key == "End")
  {
    edje->emit(signal, "dds_button");
  }
  else if(key == "Next")
  {
    edje->emit(signal, "dds_right");
  }
  else if(key == "n")
  {
    edje->emit(signal, "button_nav_overlay");
  }
  else if(key == "m")
  {
    edje->emit(signal, "button_media_overlay");
  }
  else if(key == "u")
  {
    edje->emit(signal, "button_menu_overlay");
  }
  else if(key == "p")
  {
    edje->emit(signal, "button_phone_overlay");
  }
  else if(key == "r")
  {
    edje->emit(signal, "button_return_overlay");
  }
  else if(key == "q")
  {
    quit();
  }
}

void Simulation::buttonHandler(const std::string emmision, const std::string source, enum ButtonEvent::Number number, enum ButtonEvent::Value value)
{
  ButtonEvent keyEvent;
  keyEvent.time = time(NULL);

  keyEvent.number = number;
  keyEvent.value = value;
  controlServer->onButtonListener(keyEvent);
}

void Simulation::axisHandler(const std::string emmision, const std::string source, enum AxisEvent::Number number, enum AxisEvent::Value value)
{
  AxisEvent keyEvent;
  keyEvent.time = time(NULL);

  keyEvent.number = number;
  keyEvent.value = value;
  controlServer->onAxisListener(keyEvent);
}

void Simulation::rotaryHandler(const std::string emmision, const std::string source, enum RotaryEvent::Number number, enum RotaryEvent::Value value)
{
  RotaryEvent keyEvent;
  keyEvent.time = time(NULL);

  keyEvent.number = number;
  keyEvent.value = value;
  controlServer->onRotaryListener(keyEvent);
}

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OICFControlListenerProviderImpl.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>

using namespace std;

OICFControlListenerProviderImpl::OICFControlListenerProviderImpl(DBus::Connection &connection) :
  OICFControlListenerProvider(connection)
{
  if (!joystick.open())
  {
    cerr << "No joystick device found!" << endl;
    exit(1);
  }

  joystick.signalAxis.connect(sigc::mem_fun(this, &OICFControlListenerProviderImpl::onAxisListenerWrap));
  joystick.signalButton.connect(sigc::mem_fun(this, &OICFControlListenerProviderImpl::onButtonListenerWrap));
}

void OICFControlListenerProviderImpl::onAxisListenerWrap(const EventJoystick &event)
{
  AxisEvent axisEvent;

  cout << "Axis - ";
  printf("Number: %d", event.number);
  printf(", Value: %d", event.value);
  printf(", synthetic: %d\n", event.synthetic);

  axisEvent.time = event.time;
  axisEvent.value = event.value;
  axisEvent.number = event.number;

  if ((event.number == 0) && (axisEvent.value > 0))
  {
    axisEvent.number = AxisEvent::X;
    axisEvent.value = AxisEvent::Max;
  }
  else if ((event.number == 0) && (event.value == 0))
  {
    axisEvent.number = AxisEvent::X;
    axisEvent.value = AxisEvent::Zero;
  }
  else if ((event.number == 0) && (event.value < 0))
  {
    axisEvent.number = AxisEvent::X;
    axisEvent.value = AxisEvent::Min;
  }
  else if ((event.number == 1) && (event.value > 0))
  {
    axisEvent.number = AxisEvent::Y;
    axisEvent.value = AxisEvent::Max;
  }
  else if ((event.number == 1) && (event.value == 0))
  {
    axisEvent.number = AxisEvent::Y;
    axisEvent.value = AxisEvent::Zero;
  }
  else if ((event.number == 1) && (event.value < 0))
  {
    axisEvent.number = AxisEvent::Y;
    axisEvent.value = AxisEvent::Min;
  }

  onAxisListener(axisEvent);
}

void OICFControlListenerProviderImpl::onButtonListenerWrap(const EventJoystick &event)
{
  ButtonEvent buttonEvent;

  buttonEvent.time = event.time;

  switch (event.value)
  {
  case 0:
    buttonEvent.value = ButtonEvent::Up;
    break;
  case 1:
    buttonEvent.value = ButtonEvent::Down;
    break;
  default:
    break;
  };

  cout << "Button - ";
  printf("Number: %d", event.number);
  printf(", Value: %d", event.value);
  printf(", synthetic: %d\n", event.synthetic);

  switch (event.number)
  {
  case 0:
    cout << "button 0 not yet used" << endl;
    break;

  case 1:
    buttonEvent.number = ButtonEvent::Volume;
    break;

  case 2:
    buttonEvent.number = ButtonEvent::Return;
    break;

  case 3:
    buttonEvent.number = ButtonEvent::DDS;
    break;

  case 4:
    buttonEvent.number = ButtonEvent::Navigation;
    break;

  case 5:
    buttonEvent.number = ButtonEvent::Media;
    break;

  case 6:
    buttonEvent.number = ButtonEvent::Phone;
    break;

  case 7:
    buttonEvent.number = ButtonEvent::Menu;
    break;

  default:
    break;
  };

  onButtonListener(buttonEvent);
}

std::map< std::string, std::string > OICFControlListenerProviderImpl::Info()
{
  std::map< std::string, std::string > info;
  char hostname[HOST_NAME_MAX];

  gethostname(hostname, sizeof(hostname));
  info["hostname"] = hostname;
  info["username"] = getlogin();

  return info;
}


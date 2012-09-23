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
  KeyEvent keyEvent;

  cout << "Axis - ";
  printf("Number: %d", event.number);
  printf(", Value: %d", event.value);
  printf(", synthetic: %d\n", event.synthetic);

  keyEvent.time = event.time;
  keyEvent.value = event.value;
  keyEvent.number = event.number;

  if ((event.number == 0) && (keyEvent.value > 0))
  {
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Max;
  }
  else if ((event.number == 0) && (keyEvent.value == 0))
  {
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Zero;
  }
  else if ((event.number == 0) && (keyEvent.value < 0))
  {
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Min;
  }
  else if ((event.number == 1) && (keyEvent.value > 0))
  {
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Max;
  }
  else if ((event.number == 1) && (keyEvent.value == 0))
  {
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Zero;
  }
  else if ((event.number == 1) && (keyEvent.value < 0))
  {
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Min;
  }

  onAxisListener(keyEvent);
}

void OICFControlListenerProviderImpl::onButtonListenerWrap(const EventJoystick &event)
{
  KeyEvent keyEvent;

  keyEvent.time = event.time;

  switch (event.value)
  {
  case 0:
    keyEvent.value = KeyEvent::Up;
    break;
  case 1:
    keyEvent.value = KeyEvent::Down;
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
    keyEvent.number = KeyEvent::One;
    break;

  case 1:
    keyEvent.number = KeyEvent::Two;
    break;

  case 2:
    keyEvent.number = KeyEvent::Three;
    break;

  case 3:
    keyEvent.number = KeyEvent::Four;
    break;

  case 4:
    keyEvent.number = KeyEvent::Navigation;
    break;

  case 5:
    keyEvent.number = KeyEvent::Media;
    break;

  case 6:
    keyEvent.number = KeyEvent::Test;
    break;

  case 7:
    keyEvent.number = KeyEvent::Test2;
    break;

  case 8:
    keyEvent.number = KeyEvent::Menu;
    break;

  case 9:
    keyEvent.number = KeyEvent::Start;
    break;

  default:
    break;
  };

  onButtonListener(keyEvent);
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


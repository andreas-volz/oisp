#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>

/* libudev */
#include <libudev.h>

#include "Joystick.h"

using namespace std;

Joystick::Joystick() :
  m_init(false),
  m_axes(0),
  m_buttons(0),
  m_run(true)
{
}

Joystick::~Joystick()
{
  this->close();
}

bool Joystick::open()
{
  string device = getDeviceName();

  if (!device.empty())
  {
    return open(device);
  }

  return false;
}

bool Joystick::open(const std::string &device)
{
  m_fd = ::open(device.c_str(), O_RDONLY);
  if (m_fd == -1)
  {
    cerr << "Error opening joystick device!" << endl;

    return false;
  }
  else
  {
    char buttons;
    char axes;
    char name[128];

    // get number of buttons
    ioctl(m_fd, JSIOCGBUTTONS, &buttons);
    m_buttons = buttons;

    // get number of axes
    ioctl(m_fd, JSIOCGAXES, &axes);
    m_axes = axes;

    // get device name
    if (ioctl(m_fd, JSIOCGNAME(sizeof(name)), name) < 0)
    {
      m_name = "Unknown";
    }
    else
    {
      m_name = name;
    }

    /* TODO: support those if needed
     * #define JSIOCGVERSION   // get driver version
     * #define JSIOCSCORR      // set correction values
     * #define JSIOCGCORR      // get correction values
     */

    thread = Glib::Thread::create(sigc::mem_fun(*this, &Joystick::loop), false);
    m_run = true;
  }

  return true;
}

bool Joystick::close()
{
  // end thread
  m_run = false;

  // reset some values
  m_init = false;
  m_axes = 0;
  m_buttons = 0;

  return !::close(m_fd);
}

void Joystick::loop()
{
  // wait for all synthetic event until the first real event comes
  // then we've all available axis and buttons.

  while (m_run)
  {
    EventJoystick eventJoy;

    ssize_t ignore = read(m_fd, &joy_event, sizeof(struct js_event));

    eventJoy.time = joy_event.time;
    eventJoy.value = joy_event.value;

    switch (joy_event.type)
    {
    case JS_EVENT_BUTTON:
      if (!m_init) m_init = true;
      eventJoy.number = joy_event.number;
      eventJoy.synthetic = false;
      signalButton.emit(eventJoy);
      break;

    case JS_EVENT_AXIS:
      if (!m_init) m_init = true;
      eventJoy.number = joy_event.number;
      eventJoy.synthetic = false;
      signalAxis.emit(eventJoy);
      break;

    case JS_EVENT_BUTTON | JS_EVENT_INIT:
      if (m_init) // skip the synthetic events on driver start
      {
        eventJoy.number = joy_event.number & ~JS_EVENT_INIT;
        eventJoy.synthetic = true;
        signalButton.emit(eventJoy);
      }
      break;

    case JS_EVENT_AXIS | JS_EVENT_INIT:
      if (m_init) // skip the synthetic events on driver start
      {
        eventJoy.number = joy_event.number & ~JS_EVENT_INIT;
        eventJoy.synthetic = true;
        signalAxis.emit(eventJoy);
      }
      break;

    default: // we should never reach this point
      printf("unknown event: %x\n", joy_event.type);
    }
  }
}

int Joystick::getNumberOfButtons()
{
  return m_buttons;
}

int Joystick::getNumberOfAxes()
{
  return m_axes;
}

string Joystick::getIdentifier()
{
  return m_name;
}

string Joystick::getDeviceName()
{
  struct udev *udev;
  struct udev_enumerate *enumerate;
  struct udev_list_entry *devices, *dev_list_entry;
  struct udev_device *dev;
  struct udev_list_entry *list_entry;
  const char *node = NULL;

  /* Create the udev object */
  udev = udev_new();
  if (!udev)
  {
    printf("Can't create udev\n");
    return "";
    // TODO: exception
  }

  /* Create a list of the devices in the 'input' subsystem. */
  enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);

  /* For each item enumerated, print out its information.
     udev_list_entry_foreach is a macro which expands to
     a loop. The loop will be executed for each member in
     devices, setting dev_list_entry to a list entry
     which contains the device's path in /sys. */
  udev_list_entry_foreach(dev_list_entry, devices)
  {
    const char *path;
    bool joystick = false;
    bool minor = false;

    /* Get the filename of the /sys entry for the device
       and create a udev_device object (dev) representing it */
    path = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(udev, path);

    list_entry = udev_device_get_properties_list_entry(dev);
    while (list_entry != NULL)
    {
      const char *name;
      const char *value;

      //printf("%s=%s\n", udev_list_entry_get_name(list_entry), udev_list_entry_get_value(list_entry));
      list_entry = udev_list_entry_get_next(list_entry);
      name = udev_list_entry_get_name(list_entry);
      value = udev_list_entry_get_value(list_entry);

      if (name && value)
      {
        if ((!strcmp(name, "ID_INPUT_JOYSTICK")) && (!strcmp(value, "1")))
        {
          joystick = true;
        }
        // not sure if MINOR=0 is a valid way to detect the char device to open as joystick...
        if ((!strcmp(name, "MINOR")) && (!strcmp(value, "0")))
        {
          minor = true;
        }
      }
    }

    /* usb_device_get_devnode() returns the path to the device node
       itself in /dev. */
    if (joystick && minor)
    {
      node = udev_device_get_devnode(dev);
    }
  }
  /* Free the enumerator object */
  udev_enumerate_unref(enumerate);

  // TODO: exception
  if (!node)
    return "";

  return node;
}

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <signal.h>

#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <glibmm.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "OICFControlListenerProviderImpl.h"

using namespace std;

static const char *ECHO_SERVER_NAME = "org.oicf.ControlListener";

DBus::Glib::BusDispatcher dispatcher;

gboolean on_key_changed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
static void destroy(GtkWidget *widget, gpointer data);
int create_window(int argc, char **argv);

OICFControlListenerProviderImpl *controlServer;

int main(int argc, char **argv)
{
  // initialize Glib thread system
  if (!Glib::thread_supported()) Glib::thread_init();

  DBus::default_dispatcher = &dispatcher;

  dispatcher.attach(NULL);

  DBus::Connection conn = DBus::Connection::SessionBus();

  conn.request_name(ECHO_SERVER_NAME);

  controlServer = new OICFControlListenerProviderImpl(conn);

  cout << "OICPControl_keyb server started..." << endl;

  create_window(argc, argv);

  delete controlServer;

  cout << "OICPControl_keyb server stopped..." << endl;

  return 0;
}

int create_window(int argc, char **argv)
{
  GtkWidget *window;

  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  // to be used by OILM!
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_UTILITY);

  // alpa channel
  gtk_window_set_opacity (GTK_WINDOW(window), 0.3);

  // stay on top
  gtk_window_set_keep_above (GTK_WINDOW(window), true);

  gtk_window_set_title(GTK_WINDOW(window), "Must have input focus!");

  gtk_widget_set_events(window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_changed), NULL);

  gtk_window_set_default_size(GTK_WINDOW(window), 50, 50);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);

  gtk_widget_show(window);

  gtk_main();

  return 0;
}

// http://library.gnome.org/devel/gdk/stable/gdk-Event-Structures.html#GdkEventKey
// /usr/include/gtk-2.0/gdk/gdkkeysyms.h
gboolean on_key_changed(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  printf("key: %d\n", event->keyval);

  KeyEvent keyEvent;
  keyEvent.time = event->time;

  switch (event->keyval)
  {
  case GDK_Left:
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Min;
    controlServer->onAxisListener(keyEvent);
    break;

  case GDK_Up:
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Max;
    controlServer->onAxisListener(keyEvent);
    break;

  case GDK_Right:
    keyEvent.number = KeyEvent::X;
    keyEvent.value = KeyEvent::Max;
    controlServer->onAxisListener(keyEvent);
    break;

  case GDK_Down:
    keyEvent.number = KeyEvent::Y;
    keyEvent.value = KeyEvent::Min;
    controlServer->onAxisListener(keyEvent);
    break;

  case GDK_F1:
    keyEvent.number = KeyEvent::Navigation;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F2:
    keyEvent.number = KeyEvent::Media;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F3:
    keyEvent.number = KeyEvent::Test;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F4:
    keyEvent.number = KeyEvent::Test2;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F5:
    keyEvent.number = KeyEvent::One;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F6:
    keyEvent.number = KeyEvent::Two;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F7:
    keyEvent.number = KeyEvent::Three;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F8:
    keyEvent.number = KeyEvent::Four;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F9:
    keyEvent.number = KeyEvent::Start;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_F10:
    keyEvent.number = KeyEvent::Menu;
    keyEvent.value = KeyEvent::Down;
    controlServer->onButtonListener(keyEvent);
    break;

  case GDK_q:
    gtk_main_quit();
    break;

  default:
    // not supported key combination pressed
    break;
  }



  return TRUE;
}

static void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

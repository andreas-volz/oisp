#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include <map>
#include <string>

class NavigationScreen;

using std::map;
using std::string;

// Singleton class
class ScreenManager
{
public:
  static ScreenManager &instance();

  Evasxx::Canvas *getEvasCanvas()
  {
    return evas;
  }

  void init(int argc, char **argv, const Eflxx::Size &size, bool desktop = false);

  const Eflxx::Size getSize();

  Ecorexx::Application *app;

protected:

private:
  ScreenManager() {}
  ScreenManager(const ScreenManager &);
  virtual ~ScreenManager();

  static void quit(const Ecorexx::EvasWindow &win);

  static void resize_cb(Ecore_Evas *ee);

  Eflxx::Size mSize;
  NavigationScreen *mNavigationScreen;

  Ecorexx::EvasWindowSoftwareX11 *mw;
  Evasxx::Canvas *evas;
  Evasxx::Rectangle *evas_rect;
};

#endif // SCREENMANAGER_H

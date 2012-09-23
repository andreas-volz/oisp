#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ScreenManager.h"
#include <iostream>
#include <Ecore.h>
#include "NavigationScreen.h"

using namespace std;
using namespace Eflxx;

ScreenManager &ScreenManager::instance()
{
  static ScreenManager _instance;
  return _instance;
}

ScreenManager::~ScreenManager()
{
}

void ScreenManager::init(int argc, char **argv, const Eflxx::Size &size)
{
  mNavigationScreen = NULL;

  mSize = size;

  /* Create the application object */
  app = new Ecorexx::Application(argc, (const char **) argv, "OISPMapViewer");

  /* Create the main window, a window with an embedded canvas */
  mw = new Ecorexx::EvasWindowSoftwareX11(mSize);
  mw->setAlpha(true);

  // pin on desktop: TODO: better option handling:
  // http://code.google.com/p/google-gflags/ or libgetopt++
  if ((argc >= 2) && (string(argv[1]) == "--desktop"))
  {
    Ecorexx::XWindow *xwin = mw->getXWindow();
    xwin->setNetWMWindowType(Ecorexx::XWindow::Desktop);
  }

  mw->show();

  evas = &(mw->getCanvas());

  evas->appendFontPath(PACKAGE_DATA_DIR "fonts");

  evas_rect = new Evasxx::Rectangle(*evas, mSize);
  evas_rect->setColor(Color(0, 0, 0, 0));
  evas_rect->setFocus(true);

  // destroy handler
  mw->deleteRequestSignal.connect(sigc::ptr_fun(&quit));
}

const Eflxx::Size ScreenManager::getSize()
{
  return mSize;
}

void ScreenManager::quit(const Ecorexx::EvasWindow &win)
{
  Ecorexx::Application::quit();
}

#ifndef NAVIGATIONSCREEN_H
#define NAVIGATIONSCREEN_H

#include <edjexx/Edjexx.h>
#include "Esmartxx_Cairo.h"
#include "Navigation.h"
#include "EcoreDispatcher.h"
#include "ScreenManager.h"
#include "MapCanvas.h"
#include "util.h"

class NavigationScreen
{
public:
  NavigationScreen();
  virtual ~NavigationScreen();

  enum MapMode
  {
    Drive2D,
    North2D,
    CrossCursor
  };

  void setNavigation(Navigation *navigation);

  void setCurrentGPSPositon(double lon, double lat);

  void setZoom(double zoom);

  void setMapMode(MapMode mapMode);

  MapMode getMapMode();

  void moveMap(Navigation::PanDirection pan);

  void startRouteTo(const std::string &city, const std::string &street);

protected:

private:
  void createCarPos();
  void createCrossCursor();

  bool drawerTask(Ecorexx::Timer &timer);

  void createWidgets();

  Logger mLogger;

  ScreenManager &screenManager;

  Navigation *mNavigation;

  Edjexx::Object mCarPosEdje;
  Edjexx::Object mCrossCursorEdje;

  Eflxx::Size mCarPosSize;
  MapCanvas mMapCanvas;

  Ecorexx::Timer *mDrawTimer;
#ifdef PROFILING
  ::StopClock mDrawClock;
#endif
  float mDrawTime;

  double mLon;
  double mLat;
  double mZoom;

  double mCrossCursorLon;
  double mCrossCursorLat;

  MapMode mMapMode;
};

#endif // NAVIGATIONSCREEN_H

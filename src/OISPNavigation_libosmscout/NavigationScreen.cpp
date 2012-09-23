#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "NavigationScreen.h"
#include "searchFile.h"
#include <iostream>

using namespace std;
using namespace Eflxx;

NavigationScreen::NavigationScreen() :
  screenManager(ScreenManager::instance()),
  mNavigation(NULL),
  mCarPosEdje(*(screenManager.getEvasCanvas()), searchThemeFile("enigator_default.edj"), "pointer"),
  mCrossCursorEdje(*(screenManager.getEvasCanvas()), searchThemeFile("enigator_default.edj"), "cursor"),
  mCarPosSize(64, 64),
  mMapCanvas(*(screenManager.getEvasCanvas()), screenManager.getSize()),
  mDrawTimer(NULL),
  mLon(0),
  mLat(0),
  mZoom(0),
  mCrossCursorLon(0),
  mCrossCursorLat(0),
  mMapMode(North2D)
{
  createWidgets();

  sigc::slot <bool, Ecorexx::Timer &> timerSlot = sigc::mem_fun(*this, &NavigationScreen::drawerTask);

  mDrawTimer = Ecorexx::Timer::factory(0.5, timerSlot);
}

NavigationScreen::~NavigationScreen()
{
  mDrawTimer->destroy();
}

void NavigationScreen::setNavigation(Navigation *navigation)
{
  mNavigation = navigation;
}

void NavigationScreen::setMapMode(MapMode mapMode)
{
  mMapMode = mapMode;
}

void NavigationScreen::moveMap(Navigation::PanDirection pan)
{
  int moveStep = 20;
  int x = screenManager.getSize().width() / 2;
  int y = screenManager.getSize().height() / 2;

  switch (pan)
  {
  case Navigation::North:
    y += moveStep;
    break;
  case Navigation::South:
    y -= moveStep;
    break;
  case Navigation::East:
    x += moveStep;
    break;
  case Navigation::West:
    x -= moveStep;
    break;
  }

  mMapCanvas.PixelToGeo(x, y, mCrossCursorLon, mCrossCursorLat);
}

void NavigationScreen::startRouteTo(const std::string &city, const std::string &street)
{
  mMapCanvas.startRouteTo(mLat, mLon, city, street);
}

NavigationScreen::MapMode NavigationScreen::getMapMode()
{
  return mMapMode;
}

bool NavigationScreen::drawerTask(Ecorexx::Timer &timer)
{
  cout << "+mapDrawerTask" << endl;

  // temporary zoom animation counter
  static int i = 0;
  //i += 200; // activate this to see zoom in animation

  mDrawTime = mDrawClock.getElapsedTime(::StopClock::TIME_UNIT_MILISECONDS);

  // this implements very raw a frame drop...
  // draw map only if last draw has took < n ms
  // larger time cases means system is very busy and rendering needs to long...
  if (mDrawTime < 1000)
  {
    if (mMapMode == CrossCursor)
    {
      mMapCanvas.drawMap(mCrossCursorLon, mCrossCursorLat, (mZoom + i));
    }
    else
    {
      mMapCanvas.drawMap(mLon, mLat, (mZoom + i));
    }
  }
  else
  {
    cerr << "Skip map drawing frame!" << endl;
  }
  mDrawClock.resetClock();

  double x = 0;
  double y = 0;

  // this code places car cursor simply in the middle.
  // TODO: write some code to place cursor everywhere on the screen and adjust screen GPS coord
  if (mMapCanvas.GeoToPixel(mLon, mLat, x, y))
  {
    x -= (mCarPosSize.width() / 2);
    y -= (mCarPosSize.height() / 2);
    mCarPosEdje.move(Eflxx::Point((int)x, (int)y));
  }

  if (mMapMode == CrossCursor)
  {
    mCrossCursorEdje.setVisible(true);
  }
  else
  {
    mCrossCursorEdje.setVisible(false);
  }

  return true;
}

void NavigationScreen::createWidgets()
{
  createCarPos();
  createCrossCursor();
}

void NavigationScreen::createCarPos()
{
  mCarPosEdje.resize(mCarPosSize);
  mCarPosEdje.setVisible(true);
  mCarPosEdje.setLayer(1);
}

void NavigationScreen::createCrossCursor()
{
  mCrossCursorEdje.resize(screenManager.getSize());

  mCrossCursorEdje.setVisible(true);
  mCrossCursorEdje.setLayer(1);
}

void NavigationScreen::setCurrentGPSPositon(double lon, double lat)
{
  mLon = lon;
  mLat = lat;
}

void NavigationScreen::setZoom(double zoom)
{
  cout << "setting zoom: " << zoom << endl;
  mZoom = zoom;
}

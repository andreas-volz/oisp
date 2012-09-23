#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// STD
#include <iostream>
#include <cstring>
#include <cmath>

#include "Navigation.h"
#include "NavigationScreen.h"
#include "MathUtil.h"

#ifdef PROFILING
#include <StopClock/StopClock.h>
#endif //PROFILING 

using namespace std;

Navigation::Navigation(OICFNavigationListenerProvider *mapViewerListenerProvider) :
  mGPSReceived(false),
  minMapZoom(16),
  maxMapZoom(0),
  mapZoomLevel(13),
  mNavigationListenerProvider(mapViewerListenerProvider),
  counter(0),
  mGPSTimer(NULL),
  mNavScreen(NULL)
{
  initGPS();
}

Navigation::~Navigation()
{
  mGPSTimer->destroy();
}

void Navigation::start()
{
}

void Navigation::setNavigationScreen(NavigationScreen *navScreen)
{
  mNavScreen = navScreen;

  // initialize default position
  mNavScreen->setZoom(pow(2, mapZoomLevel));
  double lon = 0.0;
  double lat = 0.0;
  getGPSPositionWGS84(lon, lat);
  mNavScreen->setCurrentGPSPositon(lon, lat);
}

void Navigation::setOICFNavigationListenerProvider(OICFNavigationListenerProvider *mapViewerListenerProvider)
{
  mNavigationListenerProvider = mapViewerListenerProvider;
}

void Navigation::initGPS()
{
  if (mGPSCon.open())
  {
    mGPSCon.query(WATCH_ENABLE | POLL_NONBLOCK);
    mGPSCon.setSignaling(true);

    mGPSCon.poll(&m_gpsData);
    dumpGPSData(&m_gpsData);

    // setup the gps handler before the first poll
    mGPSCon.signalData.connect(sigc::mem_fun(this, &Navigation::onGPSData));

    // do a first poll before the timer starts to see the position earlier
    // FIXME: so the idea, but it needs some time to show. Not sure why...
    mGPSCon.poll(&m_gpsData);

    sigc::slot <bool, Ecorexx::Timer &> timerSlot = sigc::mem_fun(*this, &Navigation::triggerGPSPoll);

    mGPSTimer = Ecorexx::Timer::factory(0.5, timerSlot);

  }
  else
  {
    cerr << "Warning: No GPS connected!" << endl;
  }
}

bool Navigation::triggerGPSPoll(Ecorexx::Timer &timer)
{
  mGPSCon.poll(&m_gpsData);

  return true;
}

void Navigation::getGPSPositionWGS84(double &outLon, double &outLat)
{
  if (mGPSReceived)
  {
    struct gps_data_t gpsData;
    struct gps_fix_t *gpsFix;

    m_mutexGPSData.lock();
    memcpy(&gpsData, &m_gpsData, sizeof(struct gps_data_t));
    m_mutexGPSData.unlock();

    gpsFix = &gpsData.fix;

    outLon = gpsFix->longitude;
    outLat = gpsFix->latitude;
  }
  else // not yet GPS locked...
  {
    // Default: Gemeinde Brachttal -> 50.302847 , 9.298067
    // Brachtpe: 51.005 , 7.79596

    double lat = 50.302847;
    double lon = 9.298067;

    outLat = lat;
    outLon = lon;
  }
}

// FIXME: GPS data is queued sometimes (e.g. while calculating route)
//        result: the GPS position is completly wrong!
//        solution: don't block the GPS data thread!!
void Navigation::onGPSData(struct gps_data_t *gpsData)
{
  cout << "onGPSData" << endl;

  if (gpsData->status == STATUS_FIX)
  {
    m_mutexGPSData.lock();
    memcpy(&m_gpsData, gpsData, sizeof(struct gps_data_t));
    m_mutexGPSData.unlock();

    //dumpGPSData (&m_gpsData);

    // set first GPS position received
    mGPSReceived = true;

    // take each nth element and redraw the map
    // this is only to smooth values and prevent jumping a little
    counter++;
    if (counter == 3)
    {
      double lon = 0.0;
      double lat = 0.0;
      getGPSPositionWGS84(lon, lat);
      mNavScreen->setCurrentGPSPositon(lon, lat);
      counter = 0;
    }

    struct gps_fix_t *gpsFix;
    gpsFix = &gpsData->fix;

    // fill oicf data structure and send through DBus...
    CoordWGS84 coord;
    coord.latitude = gpsFix->latitude;
    coord.longitude = gpsFix->longitude;
    coord.altitude = gpsFix->altitude;
    coord.speed = gpsFix->speed;
    coord.track = gpsFix->track;

    // FIXME: This is a little hack as gps updates start before DBus has started
    if (mNavigationListenerProvider)
    {
      cout << "send GPS Info to GUI" << endl;
      mNavigationListenerProvider->updateGPSPositionWGS84(coord);
    }
  }
}

void Navigation::setMapZoomLevel(int level)
{
  if (between <int> (level, minMapZoom, maxMapZoom))
  {
    mapZoomLevel = level;
    mNavScreen->setZoom(pow(2, mapZoomLevel));
  }
}

int Navigation::getMapZoomLevel() const
{
  return mapZoomLevel;
}

void Navigation::zoomIn()
{
  setMapZoomLevel(getMapZoomLevel() + 1);
}

void Navigation::zoomOut()
{
  setMapZoomLevel(getMapZoomLevel() - 1);
}

void Navigation::jumpToPointer()
{
  mNavScreen->setMapMode(NavigationScreen::North2D);
}

void Navigation::moveMap(PanDirection pan)
{
  mNavScreen->setMapMode(NavigationScreen::CrossCursor);
  mNavScreen->moveMap(pan);
}

void Navigation::startRouteTo(const std::string &city, const std::string &street)
{
  mNavScreen->startRouteTo(city, street);
}

int Navigation::getGPSHeight()
{
  if (mGPSReceived)
  {
    struct gps_data_t gpsData;
    struct gps_fix_t *gpsFix;

    m_mutexGPSData.lock();
    memcpy(&gpsData, &m_gpsData, sizeof(struct gps_data_t));
    m_mutexGPSData.unlock();

    gpsFix = &gpsData.fix;

    return static_cast<int>(gpsFix->altitude);
  }

  return 0;
}

/// DEBUG CODE BELOW HERE
void Navigation::dumpGPSData(struct gps_data_t *gpsData)
{
  cout << "struct gps_data_t:" << endl
       << "__________________" << endl;

  cout << "Status: ";
  switch (gpsData->status)
  {
  case STATUS_NO_FIX:
    cout << "No Fix" << endl;
    break;

  case STATUS_FIX:
    cout << "Fix" << endl;
    break;

  case STATUS_DGPS_FIX:
    cout << "DGPS Fix" << endl;
    break;
  }

  cout << "Satellites used: " << gpsData->satellites_used << endl;

  cout << endl << "struct gps_fix_t:" << endl
       << "_________________" << endl;

  struct gps_fix_t *gpsFix;
  gpsFix = &gpsData->fix;

  cout << "Time: " << gpsFix->time << endl;

  cout << "Mode: ";
  switch (gpsFix->mode)
  {
  case MODE_NOT_SEEN:
    cout << "Not seen" << endl;
    break;

  case MODE_NO_FIX:
    cout << "No Fix" << endl;
    break;

  case MODE_2D:
    cout << "2D" << endl;
    break;

  case MODE_3D:
    cout << "3D" << endl;
    break;
  }

  cout << "ept: " << gpsFix->ept << endl;
  cout << "latitude: " << gpsFix->latitude << endl;
  cout << "longitude: " << gpsFix->longitude << endl;
  cout << "altitude: " << gpsFix->altitude << endl;
  cout << "epv: " << gpsFix->epv << endl;
  cout << "track: " << gpsFix->track << endl;
  cout << "epd: " << gpsFix->epd << endl;
  cout << "speed: " << gpsFix->speed << endl;
  cout << "eps: " << gpsFix->eps << endl;
  cout << "climb: " << gpsFix->climb << endl;
  cout << "epc: " << gpsFix->epc << endl;

  cout << endl;
}

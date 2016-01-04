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
  mLogger("oisp.Navigation.Navigation"),
  mGPSReceived(false),
  minMapZoom(16),
  maxMapZoom(0),
  mapZoomLevel(13),
  mNavigationListenerProvider(mapViewerListenerProvider),
  counter(0),
  mGPSTimer(NULL),
  mNavScreen(NULL)
{
}

Navigation::~Navigation()
{
  if(mGPSTimer)
  {
    mGPSTimer->destroy();
  }
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

void Navigation::initGPS(const std::string &host, int port)
{
  if (mGPSCon.open(host, port))
  {
    mGPSCon.stream(WATCH_ENABLE | WATCH_JSON);
    mGPSCon.setSignaling(true);

    mGPSCon.read(&m_gpsData);
    dumpGPSData(&m_gpsData);

    // setup the gps handler before the first poll
    mGPSCon.signalData.connect(sigc::mem_fun(this, &Navigation::onGPSData));

    // do a first poll before the timer starts to see the position earlier
    // FIXME: so the idea, but it needs some time to show. Not sure why...
    mGPSCon.read(&m_gpsData);

    sigc::slot <bool, Ecorexx::Timer &> timerSlot = sigc::mem_fun(*this, &Navigation::triggerGPSPoll);

    mGPSTimer = Ecorexx::Timer::factory(0.5, timerSlot);

  }
  else
  {
    LOG4CXX_WARN(mLogger, "No GPS connected!");
  }
}

bool Navigation::triggerGPSPoll(Ecorexx::Timer &timer)
{
  mGPSCon.read(&m_gpsData);
  onGPSData(&m_gpsData);

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
    double lon = 8.298067;

    outLat = lat;
    outLon = lon;
  }
}

// FIXME: GPS data is queued sometimes (e.g. while calculating route)
//        result: the GPS position is completly wrong!
//        solution: don't block the GPS data thread!!
void Navigation::onGPSData(struct gps_data_t *gpsData)
{
  LOG4CXX_TRACE(mLogger, "onGPSData");

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
      LOG4CXX_TRACE(mLogger, "send GPS Info to GUI");
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
  LOG4CXX_INFO(mLogger, "struct gps_data_t:");
  LOG4CXX_INFO(mLogger, "__________________");

  LOG4CXX_INFO(mLogger, "Status: ");
  switch (gpsData->status)
  {
  case STATUS_NO_FIX:
    LOG4CXX_INFO(mLogger, "No Fix");
    break;

  case STATUS_FIX:
    LOG4CXX_INFO(mLogger, "Fix");
    break;

  case STATUS_DGPS_FIX:
    LOG4CXX_INFO(mLogger, "DGPS Fix");
    break;
  }

  LOG4CXX_INFO(mLogger, "Satellites used: " << gpsData->satellites_used);

  LOG4CXX_INFO(mLogger, "struct gps_fix_t:");
  LOG4CXX_INFO(mLogger, "_________________");

  struct gps_fix_t *gpsFix;
  gpsFix = &gpsData->fix;

  LOG4CXX_INFO(mLogger, "Time: " << gpsFix->time);

  LOG4CXX_INFO(mLogger, "Mode: ");
  switch (gpsFix->mode)
  {
  case MODE_NOT_SEEN:
    LOG4CXX_INFO(mLogger, "Not seen");
    break;

  case MODE_NO_FIX:
    LOG4CXX_INFO(mLogger, "No Fix");
    break;

  case MODE_2D:
    LOG4CXX_INFO(mLogger, "2D");
    break;

  case MODE_3D:
    LOG4CXX_INFO(mLogger, "3D");
    break;
  }

  LOG4CXX_INFO(mLogger, "ept: " << gpsFix->ept);
  LOG4CXX_INFO(mLogger, "latitude: " << gpsFix->latitude);
  LOG4CXX_INFO(mLogger, "longitude: " << gpsFix->longitude);
  LOG4CXX_INFO(mLogger, "altitude: " << gpsFix->altitude);
  LOG4CXX_INFO(mLogger, "epv: " << gpsFix->epv);
  LOG4CXX_INFO(mLogger, "track: " << gpsFix->track);
  LOG4CXX_INFO(mLogger, "epd: " << gpsFix->epd);
  LOG4CXX_INFO(mLogger, "speed: " << gpsFix->speed);
  LOG4CXX_INFO(mLogger, "eps: " << gpsFix->eps);
  LOG4CXX_INFO(mLogger, "climb: " << gpsFix->climb);
  LOG4CXX_INFO(mLogger, "epc: " << gpsFix->epc);

}

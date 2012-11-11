#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <vector>
#include <glibmm/thread.h>
#include <sigc++/sigc++.h>

#include "GPSConnection.h"
#include <OICFNavigation/OICFNavigationListenerProvider.h>

/* Eflxx */
#include <ecorexx/Ecorexx.h>

/* forward declarations */
class NavigationScreen;

class Navigation : public sigc::trackable
{
public:
  Navigation(OICFNavigationListenerProvider *mapViewerListenerProvider);
  ~Navigation();

  // TODO: move to OICF!!
  enum PanDirection
  {
    North,
    East,
    West,
    South
  };

  void initGPS(const std::string &host, int port);
  
  void start();

  void setOICFNavigationListenerProvider(OICFNavigationListenerProvider *mapViewerListenerProvider);

  void setNavigationScreen(NavigationScreen *navScreen);

  void getGPSPositionWGS84(double &outLon, double &outLat);

  int getGPSHeight();

  void setMapZoomLevel(int level);
  int getMapZoomLevel() const;

  //bool isFollowGPS ();

  /*! Increment level of map zoom.
   *  Does nothing if zoom is at maximum level.
   */
  void zoomIn();

  /*! Decrement level of map zoom.
   *  Does nothing if yet at minimum level.
   */
  void zoomOut();

  /*! set the map position to the pointer (GPS) position.
   */
  void jumpToPointer();

  void moveMap(PanDirection pan);

  void startRouteTo(const std::string &city, const std::string &street);

  sigc::signal <void> signalMapChanged;
  sigc::signal <void> signalCarPosChanged;

protected:

private:
  void onGPSData(struct gps_data_t *gpsData);
  void dumpGPSData(struct gps_data_t *gpsData);

  bool triggerGPSPoll(Ecorexx::Timer &timer);

  GPSConnection mGPSCon;

  Glib::Mutex m_mutexGPSData;

  struct gps_data_t m_gpsData;
  bool mGPSReceived;
  int minMapZoom;
  int maxMapZoom;
  int mapZoomLevel;
  OICFNavigationListenerProvider *mNavigationListenerProvider;
  int counter;
  Ecorexx::Timer *mGPSTimer;

  NavigationScreen *mNavScreen;
};

#endif // NAVIGATION_H

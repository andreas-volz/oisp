#ifndef MAP_CANVAS_H
#define MAP_CANVAS_H

/* libosmscout */
#include <osmscout/Database.h>
#include <osmscout/Router.h>
#include <osmscout/MapPainterCairo.h>
#include <osmscout/StyleConfigLoader.h>


/* Eflxx */
#include "Esmartxx_Cairo.h"
#include <eflxx/Eflxx.h>
#include <ecorexx/Ecorexx.h>

#include <StopClock/StopClock.h>

class MapCanvas : public Esmartxx::Cairo
{
public:
  MapCanvas(Evasxx::Canvas &canvas, const Eflxx::Size &size);
  ~MapCanvas();

  bool GeoToPixel(double lon, double lat, double &x, double &y);
  bool PixelToGeo(double x, double y, double &lon, double &lat);

  void drawMap(double lon, double lat, double zoom);

  void startRouteTo(double lat, double lon, const std::string &city, const std::string &street);

private:
  osmscout::DatabaseParameter mDatabaseParameter;
  osmscout::RouterParameter mRouterParameter;
  osmscout::Database mDatabase;
  osmscout::Router mRouter;
  osmscout::StyleConfig *mStyleConfig;
  osmscout::MapPainterCairo mPainter;
  osmscout::MapData mMapData;
  osmscout::MercatorProjection mProjection;
  osmscout::MapParameter mParameter;
  osmscout::AreaSearchParameter mAreaSearchParameter;
  osmscout::FastestPathRoutingProfile mRoutingProfile;
    
  cairo_surface_t *mCairoSurface;
  cairo_t *mCairo;

  void initOSMScout();

  void calcAndDrawRoute(osmscout::WayRef &wayStart, osmscout::WayRef &wayTarget);

  void calcAndDrawRoute(osmscout::WayRef &wayStart, osmscout::Point &wayStartPoint, osmscout::WayRef &wayTarget);

  bool searchWay(double latTop, double lonLeft, double latBottom, double lonRight, const std::list<std::string> &typeNames, osmscout::WayRef &foundWay, osmscout::Point &foundWayPoint);

  bool searchWay(const std::string &city, const std::string &street, osmscout::WayRef &foundWay);

  void printRouteList(osmscout::RouteData &route);

  void createSurface();

  void destroySurface();

  static std::string createHash(std::string str);

  Glib::ustring calcNextValidCharacters(const std::list <osmscout::Location> &locations, const std::string &search);

  Eflxx::Size mSize;

  // from theory it shouldn't be needed to guard position with a mutex as it's
  // only called inside ecore main loop, but it doesn't hurt and it's more save...
  Glib::Mutex mMutexPosition;

  std::list<std::string> mStreetTypeNames;
};

#endif // MAP_CANVAS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <stdexcept>

/* Glib */
#include <glibmm.h>

/* Local */
#include "MapCanvas.h"
#include "../common/util.h"
#include "Vector2.h"
#include "Preferences.h"

#ifdef PROFILING
// StopClock
#include <StopClock/StopClock.h>
#endif

using namespace std;
using namespace osmscout;



MapCanvas::MapCanvas(Evasxx::Canvas &canvas, const Eflxx::Size &size) :
  Esmartxx::Cairo(canvas, size, true),
  mLogger("oisp.Navigation.MapCanvas"),
  mDatabase(std::make_shared<osmscout::Database>(mDatabaseParameter)),
  //mRoutingProfile(mDatabase->GetTypeConfig()),
  mStyleConfig(NULL),
  mLocationService(std::make_shared<osmscout::LocationService>(mDatabase)),
  mMapService(std::make_shared<osmscout::MapService>(mDatabase)),
  mPainter(NULL),  
  mCairoSurface(NULL),
  mCairo(NULL),  
  mSize(size)
{  
  // this sets the widget itsef to visible
  setVisible(true);

  createSurface();

  initOSMScout();

  // TODO: optimize add length of mTypeNames
  mStreetTypeNames.push_back("highway_motorway");
  mStreetTypeNames.push_back("highway_motorway_link");
  mStreetTypeNames.push_back("highway_trunk");
  mStreetTypeNames.push_back("highway_trunk_link");
  mStreetTypeNames.push_back("highway_primary");
  mStreetTypeNames.push_back("highway_primary_link");
  mStreetTypeNames.push_back("highway_secondary");
  mStreetTypeNames.push_back("highway_secondary_link");
  mStreetTypeNames.push_back("highway_tertiary");
  mStreetTypeNames.push_back("highway_unclassified");
  mStreetTypeNames.push_back("highway_road");
  mStreetTypeNames.push_back("highway_residential");
  mStreetTypeNames.push_back("highway_living_street");
  mStreetTypeNames.push_back("highway_service");
  mStreetTypeNames.push_back("highway_track");
  mStreetTypeNames.push_back("highway_pedestrian");
}

MapCanvas::~MapCanvas()
{
  destroySurface();

  if (mPainter != NULL) {
    delete mPainter;
  }
}

void MapCanvas::createSurface()
{
  mCairoSurface = getSurface();
  assert(mCairoSurface);

  mCairo = cairo_create(mCairoSurface);
  assert(mCairo);
}

void MapCanvas::destroySurface()
{
  cairo_destroy(mCairo);
}

void MapCanvas::initOSMScout()
{
  Preferences &preferences = Preferences::instance ();

  mMapFolder = preferences.getNaviMapFolder();
  LOG4CXX_INFO(mLogger,  "load map from folder: " <<  mMapFolder);
  
  std::string style(mMapFolder + "/standard.oss");
	  
  if (!mDatabase->Open(mMapFolder))
  {
    LOG4CXX_ERROR(mLogger, "Cannot open database: " + mMapFolder);
    exit(1);
    // TODO: throw Exception
  }

  osmscout::TypeConfigRef typeConfig = mDatabase->GetTypeConfig();

  if (!typeConfig) {
    return;
  }
  
  osmscout::StyleConfigRef newStyleConfig=std::make_shared<osmscout::StyleConfig>(typeConfig);


  if (!newStyleConfig->Load(style))
  {
    LOG4CXX_ERROR(mLogger, "Cannot open style: " + style);
    exit(1);
    // TODO: throw Exception
  }

  mStyleConfig = newStyleConfig;

  mPainter = new MapPainterCairo(mStyleConfig);
}

void MapCanvas::GetCarSpeedTable(std::map<std::string,double>& map)
{
  map["highway_motorway"]=110.0;
  map["highway_motorway_trunk"]=100.0;
  map["highway_motorway_primary"]=70.0;
  map["highway_motorway_link"]=60.0;
  map["highway_motorway_junction"]=60.0;
  map["highway_trunk"]=100.0;
  map["highway_trunk_link"]=60.0;
  map["highway_primary"]=70.0;
  map["highway_primary_link"]=60.0;
  map["highway_secondary"]=60.0;
  map["highway_secondary_link"]=50.0;
  map["highway_tertiary_link"]=55.0;
  map["highway_tertiary"]=55.0;
  map["highway_unclassified"]=50.0;
  map["highway_road"]=50.0;
  map["highway_residential"]=40.0;
  map["highway_roundabout"]=40.0;
  map["highway_living_street"]=10.0;
  map["highway_service"]=30.0;
}

void MapCanvas::startRouteTo(double lat, double lon, const std::string &city, const std::string &street)
{
  LOG4CXX_TRACE(mLogger,  "startRouteTo: lat=" << lat << " lon=" << " city=" << city << " street=" << street);

  osmscout::WayRef way;
  bool wayAdrFound = searchWay(city, street, way);
  if (!wayAdrFound)
  {
    LOG4CXX_ERROR(mLogger, "no target way found!");
  }
  
  osmscout::ObjectFileRef targetObject;
  size_t                  targetNodeIndex;
  GeoCoord center;

  if(!way->GetCenter(center))
  {
    LOG4CXX_ERROR(mLogger, "no way center found");
    return; // FIXME: return state
  }
  
  startRouting(lat, lon, center.GetLat(), center.GetLon());
}

bool MapCanvas::searchWay(const std::string &city, const std::string &street, osmscout::WayRef &foundWay)
{
  std::list <Location> streets;
  bool limitReached;
  Glib::ustring nextValidCharacters;
  int limit = 1; // max number of results
  bool startWith = false;
  bool found = false;
  string searchPattern = city + " " + street;

  osmscout::LocationSearch                                search;
  osmscout::LocationSearchResult                          searchResult;
  std::map<osmscout::FileOffset,osmscout::AdminRegionRef> adminRegionMap;
  std::string                                             path;
  osmscout::WayRef way;
  
  search.limit=50;

  if (!mLocationService->InitializeLocationSearchEntries(searchPattern,
                                                        search)) {
    LOG4CXX_ERROR(mLogger, "Error while parsing search string: " + searchPattern);
    return false;
  }

  if (!mLocationService->SearchForLocations(search,
                                           searchResult)) {
    LOG4CXX_ERROR(mLogger, "Error while searching for location");
    found = false;
  }
  else
  {
    found = true;
  }
  
  for (const auto &entry : searchResult.results)
  {
    if (entry.adminRegion &&
        entry.location &&
        entry.address)
    {
      LOG4CXX_TRACE(mLogger, "Location/Address found");

      break; // ==> as test break after first result and use it => FIXME
    }
    else if (entry.adminRegion &&
             entry.location)
    {
      LOG4CXX_TRACE(mLogger, "Address found: " + entry.location->name);

      for (const auto &object : entry.location->objects)
      {
        if (mDatabase->GetWayByOffset(object.GetFileOffset(),
                                way))
        {
          foundWay = way;
          break; // FIXME: break after first hit 
        }
      }
    }  
    else if (entry.adminRegion &&
             entry.poi)
    {
      LOG4CXX_TRACE(mLogger, "POI found");
    }
    else if (entry.adminRegion)
    {
      LOG4CXX_TRACE(mLogger, "AdminRegion found");
    }
  }

  return found;
}

void MapCanvas::GeoToPixel(double lon, double lat, double &x, double &y)
{
  mProjection.GeoToPixel(lon, lat, x, y);
}

bool MapCanvas::PixelToGeo(double x, double y, double &lon, double &lat)
{
  return mProjection.PixelToGeo(x, y, lon, lat);
}

void MapCanvas::drawMap(double lon, double lat, double zoom)
{
  std::string output;
  bool writePNG = false;
  
  static const double DPI=96.0;
  
  osmscout::MapParameter        drawParameter;
  osmscout::AreaSearchParameter searchParameter;
    
  LOG4CXX_TRACE(mLogger, "drawing with: lat=" << lat << " lon=" << lon << " zoom=" << zoom);

  mProjection.Set(lon, lat, osmscout::Magnification(zoom), DPI, mSize.width(), mSize.height());

  std::list<osmscout::TileRef> tiles;

  
  mMapService->LookupTiles(mProjection, tiles);
  if(!mMapService->LoadMissingTileData(searchParameter, *mStyleConfig, tiles))
  {
    LOG4CXX_ERROR(mLogger, "Loading of data has error or was interrupted");
  }
  mMapService->ConvertTilesToMapData(tiles, mMapData);

  if (mPainter->DrawMap(mProjection,
                       drawParameter,
                       mMapData,
                       mCairo))
  {
    if (writePNG)
    {
      static int i = 0;
      
      if (cairo_surface_write_to_png(mCairoSurface, (toString(i + 1) + ".png").c_str()) != CAIRO_STATUS_SUCCESS)
      {
        LOG4CXX_ERROR(mLogger, "Cannot write PNG");
      }
      ++i;
    }
  }
  
  setDirty();
}

std::string MapCanvas::createHash(std::string str)
{
  Glib::ustring ustr(str);
  return ustr.casefold_collate_key();
}

Glib::ustring MapCanvas::calcNextValidCharacters(const std::list <osmscout::Location> &locations, const std::string &search)
{
  Glib::ustring locationNameUc(search);
  int locationNameUcLength(locationNameUc.length());
  Glib::ustring nextValidCharacters;
  std::string::size_type loc = std::string::npos;

  for (std::list <Location>::const_iterator lit = locations.begin();
       lit != locations.end();
       ++lit)
  {
    Glib::ustring ucstr(lit->name);
    Glib::ustring foundLoc;

    // only debug
    LOG4CXX_TRACE(mLogger, "Found Location: " + lit->name);

    loc = ucstr.find(locationNameUc);

    try
    {
      foundLoc = ucstr.substr(loc + locationNameUcLength, 1);  // TODO: not +1 => use iterator!

      if (nextValidCharacters.find(foundLoc) == std::string::npos)
      {
        nextValidCharacters.append(foundLoc);
      }
    }
    catch (std::out_of_range ex)
    {
      LOG4CXX_TRACE(mLogger, "No next char");
    }
  }

  return nextValidCharacters;
}

bool MapCanvas::searchWay(double latTop, double lonLeft, double latBottom, double lonRight, const std::list<std::string> &typeNames, osmscout::WayRef &foundWay, osmscout::Point &foundWayPoint)
{
  bool ret = false;
  //TBD
  return ret;
}

void MapCanvas::startRouting(double startLat, double startLon,
                            double targetLat, double targetLon) 
{
  std::string                               routerFilenamebase=osmscout::RoutingService::DEFAULT_FILENAME_BASE;
  osmscout::Vehicle                         vehicle=osmscout::vehicleCar;
  bool                                      outputGPX=false;

  osmscout::FastestPathRoutingProfile routingProfile(mDatabase->GetTypeConfig());
  osmscout::RouterParameter           routerParameter;

  if (!outputGPX) {
    routerParameter.SetDebugPerformance(true);
  }

  osmscout::RoutingServiceRef router=std::make_shared<osmscout::RoutingService>(mDatabase,
                                                                                routerParameter,
                                                                                routerFilenamebase);

  if (!router->Open()) {
    LOG4CXX_ERROR(mLogger, "Cannot open routing database");

    exit(1);
  }

  osmscout::TypeConfigRef             typeConfig=mDatabase->GetTypeConfig();
  osmscout::RouteData                 data;
  osmscout::RouteDescription          description;
  std::map<std::string,double>        carSpeedTable;

  switch (vehicle) {
  case osmscout::vehicleFoot:
    routingProfile.ParametrizeForFoot(*typeConfig,
                                      5.0);
    break;
  case osmscout::vehicleBicycle:
    routingProfile.ParametrizeForBicycle(*typeConfig,
                                         20.0);
    break;
  case osmscout::vehicleCar:
    GetCarSpeedTable(carSpeedTable);
    routingProfile.ParametrizeForCar(*typeConfig,
                                     carSpeedTable,
                                     160.0);
    break;
  }

  osmscout::ObjectFileRef startObject;
  size_t                  startNodeIndex;

  if (!router->GetClosestRoutableNode(startLat,
                                      startLon,
                                      vehicle,
                                      1000,
                                      startObject,
                                      startNodeIndex)) {
    LOG4CXX_ERROR(mLogger, "Error while searching for routing node near start location!");
    exit(1);
  }

  if (startObject.Invalid() || startObject.GetType()==osmscout::refNode) {
    LOG4CXX_ERROR(mLogger, "Cannot find start node for start location!");
  }

  osmscout::ObjectFileRef targetObject;
  size_t                  targetNodeIndex;

  if (!router->GetClosestRoutableNode(targetLat,
                                      targetLon,
                                      vehicle,
                                      1000,
                                      targetObject,
                                      targetNodeIndex)) {
    LOG4CXX_ERROR(mLogger, "Error while searching for routing node near target location!");
    exit(1);
  }

  if (targetObject.Invalid() || targetObject.GetType()==osmscout::refNode) {
    LOG4CXX_ERROR(mLogger, "Cannot find start node for target location!");
  }

  if (!router->CalculateRoute(routingProfile,
                              startObject,
                              startNodeIndex,
                              targetObject,
                              targetNodeIndex,
                              data)) {
    LOG4CXX_ERROR(mLogger, "There was an error while calculating the route!");
    router->Close();
    exit(1);
  }

  if (data.IsEmpty()) {
    LOG4CXX_ERROR(mLogger, "No Route found!");

    router->Close();

    exit(1);
  }

  // draw the route in map
  Way routingWay;
  router->TransformRouteDataToWay(data, routingWay);

  mMapData.poiWays.clear();
  mMapData.poiWays.push_back(std::make_shared<osmscout::Way>(routingWay));
}


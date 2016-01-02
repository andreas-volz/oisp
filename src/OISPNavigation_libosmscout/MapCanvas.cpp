#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <stdexcept>

/* Glib */
#include <glibmm.h>

/* Local */
#include "MapCanvas.h"
#include "util.h"
#include "Vector2.h"
#include "Preferences.h"
//#include "MemoryUtil.h"

#ifdef PROFILING
// StopClock
#include <StopClock/StopClock.h>
#endif

using namespace std;
using namespace osmscout;



MapCanvas::MapCanvas(Evasxx::Canvas &canvas, const Eflxx::Size &size) :
  Esmartxx::Cairo(canvas, size, true),
  //mMapFolder(string(PACKAGE_DATA_DIR) +  "/osmscout/map/current/"),
  mMapFolder("/home/andreas/.osmscout/map/current"),
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
  std::string style(mMapFolder + "/standard.oss");
	
  Preferences &preferences = Preferences::instance ();
  //std::string map (preferences.getNaviMapFolder());
  

  if (!mDatabase->Open(mMapFolder))
  {
    std::cerr << "Cannot open database: " << mMapFolder << std::endl;
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
    std::cerr << "Cannot open style: " << style << std::endl;
    exit(1);
    // TODO: throw Exception
  }

  mStyleConfig = newStyleConfig;

  mPainter = new MapPainterCairo(mStyleConfig);

  /*mRouter = std::make_shared<osmscout::RoutingService>(mDatabase,
                                                       mRouterParameter,
                                                       osmscout::RoutingService::DEFAULT_FILENAME_BASE);

  
  if (!mRouter->Open())
  {
    std::cerr << "Cannot open routing" << std::endl;
    exit(1);
    // TODO: throw Exception
  }*/

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
  cout << "startRouteTo: lat=" << lat << " lon=" << lon << " city=" << city << " street=" << street << endl;

  osmscout::WayRef way;
  bool wayAdrFound = searchWay(city, street, way);
  if (!wayAdrFound)
  {
    cerr << "no target way found!" << endl;
  }
  
  osmscout::ObjectFileRef targetObject;
  size_t                  targetNodeIndex;
  GeoCoord center;

  if(!way->GetCenter(center))
  {
    exit(1);
  }

  cout << "target: lat=" << center.GetLat() << " lon=" << center.GetLon() << endl;
  
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
    std::cerr << "Error while parsing search string" << std::endl;
    return false;
  }

  if (!mLocationService->SearchForLocations(search,
                                           searchResult)) {
    std::cerr << "Error while searching for location" << std::endl;
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
      cout << "Location/Address" << endl;

      break; // ==> as test break after first result and use it => FIXME
    }
    else if (entry.adminRegion &&
             entry.location)
    {
      cout << "Address: " << entry.location->name << endl;

      for (const auto &object : entry.location->objects)
      {
        if (mDatabase->GetWayByOffset(object.GetFileOffset(),
                                way))
        {
          GeoCoord center;

          if(!way->GetCenter(center))
          {
            exit(1);
          }

          cout << "target: lat=" << center.GetLat() << " lon=" << center.GetLon() << endl;
          
          foundWay = way;
          break; // FIXME: break after first hit 
        }
      }
    }  
    else if (entry.adminRegion &&
             entry.poi)
    {
      cout << "POI" << endl;
    }
    else if (entry.adminRegion)
    {
      cout << "AdminRegion" << endl;
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
    
  cout << "drawing with: lat=" << lat << " lon=" << lon << " zoom=" << zoom << endl;

  mProjection.Set(lon, lat, osmscout::Magnification(zoom), DPI, mSize.width(), mSize.height());

  std::list<osmscout::TileRef> tiles;

  
  mMapService->LookupTiles(mProjection, tiles);
  if(!mMapService->LoadMissingTileData(searchParameter, *mStyleConfig, tiles))
  {
    cerr << "*** Loading of data has error or was interrupted" << endl;
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
        std::cerr << "Cannot write PNG" << std::endl;
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
    cout << "Found Location: " << lit->name << endl;

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
      cout << "No next char" << endl;
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
    std::cerr << "Cannot open routing database" << std::endl;

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
    std::cerr << "Error while searching for routing node near start location!" << std::endl;
    exit(1);
  }

  if (startObject.Invalid() || startObject.GetType()==osmscout::refNode) {
    std::cerr << "Cannot find start node for start location!" << std::endl;
  }

  osmscout::ObjectFileRef targetObject;
  size_t                  targetNodeIndex;

  if (!router->GetClosestRoutableNode(targetLat,
                                      targetLon,
                                      vehicle,
                                      1000,
                                      targetObject,
                                      targetNodeIndex)) {
    std::cerr << "Error while searching for routing node near target location!" << std::endl;
    exit(1);
  }

  if (targetObject.Invalid() || targetObject.GetType()==osmscout::refNode) {
    std::cerr << "Cannot find start node for target location!" << std::endl;
  }

  if (!router->CalculateRoute(routingProfile,
                              startObject,
                              startNodeIndex,
                              targetObject,
                              targetNodeIndex,
                              data)) {
    std::cerr << "There was an error while calculating the route!" << std::endl;
    router->Close();
    exit(1);
  }

  if (data.IsEmpty()) {
    std::cout << "No Route found!" << std::endl;

    router->Close();

    exit(1);
  }

  // draw the route in map
  Way routingWay;
  router->TransformRouteDataToWay(data, routingWay);

  mMapData.poiWays.clear();
  mMapData.poiWays.push_back(std::make_shared<osmscout::Way>(routingWay));
}


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

#ifdef PROFILING
// StopClock
#include <StopClock/StopClock.h>
#endif

using namespace std;
using namespace osmscout;

MapCanvas::MapCanvas(Evasxx::Canvas &canvas, const Eflxx::Size &size) :
  Esmartxx::Cairo(canvas, size, true),
  mDatabase(mDatabaseParameter),
  mRouter(mRouterParameter),
  mStyleConfig(NULL),
  mCairoSurface(NULL),
  mCairo(NULL),
  mSize(size)
{
  // this sets the widget itsef to visible
  setVisible(true);

  createSurface();

  initOSMScout();

  // activate debug
  mDatabaseParameter.SetDebugPerformance(true);
  mParameter.SetDebugPerformance(true);

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
  free(mStyleConfig);
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

  // TODO: correct cleanup of osmscout needed
}

void MapCanvas::initOSMScout()
{
  Preferences &preferences = Preferences::instance ();
  std::string map (preferences.getNaviMapFolder());
  std::string style(map + "/standard.oss");

  if (!mDatabase.Open(map.c_str(), &MapCanvas::createHash))
  {
    std::cerr << "Cannot open database: " << map << std::endl;
    exit(1);
    // TODO: throw Exception
  }

  mStyleConfig = new StyleConfig(mDatabase.GetTypeConfig());  // TODO: smartptr

  if (!LoadStyleConfig(style.c_str(), *mStyleConfig))
  {
    std::cerr << "Cannot open style: " << style << std::endl;
    // TODO: throw Exception
  }

  if (!mRouter.Open(map.c_str()/*, &MapCanvas::createHash*/))
  {
    std::cerr << "Cannot open routing" << std::endl;
    exit(1);
    // TODO: throw Exception
  }

  osmscout::TypeId                    type;
  osmscout::TypeConfig                *typeConfig=mRouter.GetTypeConfig();
  
  mRoutingProfile.SetVehicleMaxSpeed(160.0);

  type=typeConfig->GetWayTypeId("highway_motorway");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,110.0);

  type=typeConfig->GetWayTypeId("highway_motorway_link");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,60.0);

  type=typeConfig->GetWayTypeId("highway_trunk");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,70.0);

  type=typeConfig->GetWayTypeId("highway_trunk_link");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,70.0);

  type=typeConfig->GetWayTypeId("highway_primary");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,70.0);

  type=typeConfig->GetWayTypeId("highway_primary_link");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,60.0);

  type=typeConfig->GetWayTypeId("highway_secondary");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,60.0);

  type=typeConfig->GetWayTypeId("highway_secondary_link");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,50.0);

  type=typeConfig->GetWayTypeId("highway_tertiary");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,55.0);

  type=typeConfig->GetWayTypeId("highway_unclassified");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,50.0);

  type=typeConfig->GetWayTypeId("highway_road");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,50.0);

  type=typeConfig->GetWayTypeId("highway_residential");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,40.0);

  type=typeConfig->GetWayTypeId("highway_living_street");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,10.0);

  type=typeConfig->GetWayTypeId("highway_service");
  assert(type!=osmscout::typeIgnore);
  mRoutingProfile.AddType(type,30.0);

}

void MapCanvas::startRouteTo(double lat, double lon, const std::string &city, const std::string &street)
{
  osmscout::WayRef currentWay;
  osmscout::Point foundWayPoint;
  bool wayWGSFound = searchWay(lat, lon, lat, lon, mStreetTypeNames, currentWay, foundWayPoint);
  if (!wayWGSFound)
  {
    cerr << "no start way found!" << endl;
  }

  osmscout::WayRef startWay;
  osmscout::WayRef targetWay;

  //searchWay ("Brachttal", "Gärtnerweg", startWay);
  //searchWay ("Langen (Hessen)", "Robert-Bosch-Straße", targetWay);

  bool wayAdrFound = searchWay(city, street, targetWay);
  if (!wayAdrFound)
  {
    cerr << "no target way found!" << endl;
  }

  if (wayWGSFound && wayAdrFound)
  {
    calcAndDrawRoute(currentWay, foundWayPoint, targetWay);
  }
}

bool MapCanvas::searchWay(const std::string &city, const std::string &street, osmscout::WayRef &foundWay)
{
  std::list <Location> streets;
  bool limitReached;
  Glib::ustring nextValidCharacters;
  int limit = 1; // max number of results
  bool startWith = false;
  bool found = false;

  std::list <osmscout::AdminRegion> regions;

  if (mDatabase.GetMatchingAdminRegions(city, regions, limit, limitReached, startWith))
  {
    const osmscout::AdminRegion &region = *(regions.begin());

    cout << "admin region:" << region.name << endl;

    /// search street -->
    if (mDatabase.GetMatchingLocations(region, street, streets,
                                       limit, limitReached, true))
    {
      osmscout::WayRef wayReference;

      // search way reference
      for (std::list<ObjectFileRef>::const_iterator or_it = (*streets.begin()).references.begin();
           or_it != (*streets.begin()).references.end();
           ++or_it)
      {
        const osmscout::ObjectFileRef &reference = *or_it;

        if (reference.GetType() == osmscout::refNode)
        {
          // ignore for way finding
        }
        else if (reference.GetType() == osmscout::refWay)
        {
          if (mDatabase.GetWayByOffset(reference.GetFileOffset(), wayReference))
          {
            cout << "Found Way: " << wayReference->GetName() << endl;

            foundWay = wayReference;
            found = true;
            break; // leave for-loop
          }
        }
        else if (reference.GetType() == osmscout::refRelation)
        {
          // ignore for way finding
        }
        else
        {
          assert(false);
        }
      }
    }
    // <--
  }

  return found;
}

void MapCanvas::calcAndDrawRoute(osmscout::WayRef &wayStart, osmscout::WayRef &wayTarget)
{
  calcAndDrawRoute(wayStart, (*wayStart->nodes.begin()), wayTarget);
}

void MapCanvas::calcAndDrawRoute(osmscout::WayRef &wayStart, osmscout::Point &wayStartPoint, osmscout::WayRef &wayTarget)
{
  RouteData route;

  cout << "Route from (way/node): " << wayStart->GetId() << "/" << (*wayStart->nodes.begin()).GetId() << endl;
  cout << "Route to (way/node): " << wayTarget->GetId() << "/" << (*wayTarget->nodes.begin()).GetId() << endl;

#ifdef PROFILING
  StopClock sc;
#endif
  mRouter.CalculateRoute(mRoutingProfile, wayStart->GetId(), wayStartPoint.GetId(), wayTarget->GetId(), (*wayTarget->nodes.begin()).GetId(), route);
#ifdef PROFILING
  cout << "CalculateRoute needs " << sc.getElapsedTime(StopClock::TIME_UNIT_SECONDS) << " sec" << endl;
#endif
  // draw the route in map
  Way routingWay;
  mRouter.TransformRouteDataToWay(route, routingWay);

  mMapData.poiWays.clear();

  osmscout::WayRef wayRef (&routingWay);
  mMapData.poiWays.push_back(wayRef);

  // show description
  printRouteList(route);
}

bool MapCanvas::GeoToPixel(double lon, double lat, double &x, double &y)
{
  return mProjection.GeoToPixel(lon, lat, x, y);
}

bool MapCanvas::PixelToGeo(double x, double y, double &lon, double &lat)
{
  return mProjection.PixelToGeo(x, y, lon, lat);
}

void MapCanvas::drawMap(double lon, double lat, double zoom)
{
  std::string output;
  bool writePNG = false;

  cout << "drawing with: lat=" << lat << " lon=" << lon << " zoom=" << zoom << endl;

  mProjection.Set(lon, lat, zoom, mSize.width(), mSize.height());

  osmscout::TypeSet nodeTypes;
  std::vector<osmscout::TypeSet> wayTypes;
  osmscout::TypeSet areaTypes;

  mStyleConfig->GetNodeTypesWithMaxMag(mProjection.GetMagnification(), nodeTypes);

  mStyleConfig->GetWayTypesByPrioWithMaxMag(mProjection.GetMagnification(), wayTypes);

  mStyleConfig->GetAreaTypesWithMaxMag(mProjection.GetMagnification(), areaTypes);


  mAreaSearchParameter.SetMaximumAreaLevel(zoom);
  mAreaSearchParameter.SetMaximumNodes(2000);
  mAreaSearchParameter.SetMaximumWays(2000);
  mAreaSearchParameter.SetMaximumAreas(std::numeric_limits<size_t>::max());

  mDatabase.GetObjects(nodeTypes,
                       wayTypes,
                       areaTypes,
                       mProjection.GetLonMin(),
                       mProjection.GetLatMin(),
                       mProjection.GetLonMax(),
                       mProjection.GetLatMax(),
                       mProjection.GetMagnification(),
                       mAreaSearchParameter,
                       mMapData.nodes,
                       mMapData.ways,
                       mMapData.areas,
                       mMapData.relationWays,
                       mMapData.relationAreas);

  if (mPainter.DrawMap(*mStyleConfig,
                       mProjection,
                       mParameter,
                       mMapData,
                       mCairo))
  {
    if (writePNG)
    {
      static int i = 0;
      ++i;
      if (cairo_surface_write_to_png(mCairoSurface, (toString(i + 1) + ".png").c_str()) != CAIRO_STATUS_SUCCESS)
      {
        std::cerr << "Cannot write PNG" << std::endl;
      }
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
  std::string map;
  TypeSet types;
  bool ret = false;

  double latMedium = (latTop + latBottom) / 2.0;
  double lonMedium = (lonLeft + lonRight) / 2.0;
  double maxDistance = numeric_limits<double>::max();

  Vector2d currentPosVec(latTop, lonLeft);

  std::cout << "- Search area: ";
  std::cout << "[" << std::min(latTop, latBottom) << "," << std::min(lonLeft, lonRight) << "]";
  std::cout << "x";
  std::cout << "[" << std::max(latTop, latBottom) << "," << std::max(lonLeft, lonRight) << "]" << std::endl;

  //types.reserve(typeNames.size() * 3); // Avoid dynamic resize

  for (std::list<std::string>::const_iterator name = typeNames.begin();
       name != typeNames.end();
       name++)
  {
    osmscout::TypeId nodeType;
    osmscout::TypeId wayType;
    osmscout::TypeId areaType;

    nodeType = mDatabase.GetTypeConfig()->GetNodeTypeId(*name);
    wayType = mDatabase.GetTypeConfig()->GetWayTypeId(*name);
    areaType = mDatabase.GetTypeConfig()->GetAreaTypeId(*name);

    if (nodeType == osmscout::typeIgnore &&
        wayType == osmscout::typeIgnore &&
        areaType == osmscout::typeIgnore)
    {
      std::cerr << "Cannot resolve type name '" << *name << "'" << std::endl;
      continue;
    }

    std::cout << "- Searching for '" << *name << "' as";

    if (nodeType != osmscout::typeIgnore)
    {
      std::cout << " node (" << nodeType << ")";

      types.SetType(nodeType);
    }

    if (wayType != osmscout::typeIgnore)
    {
      std::cout << " way (" << wayType << ")";

      if (wayType != nodeType)
      {
        types.SetType(wayType);
      }
    }

    if (areaType != osmscout::typeIgnore)
    {
      std::cout << " area (" << areaType << ")";

      if (areaType != nodeType && areaType != wayType)
      {
        types.SetType(areaType);
      }
    }

    std::cout << std::endl;
  }

  std::vector<osmscout::NodeRef> nodes;
  std::vector<osmscout::WayRef> ways;
  std::vector<osmscout::WayRef> areas;
  std::vector<osmscout::RelationRef> relationWays;
  std::vector<osmscout::RelationRef> relationAreas;
  
  osmscout::TagId nameTagId = mDatabase.GetTypeConfig()->GetTagId("name");

    if (!mDatabase.GetObjects(std::min(lonLeft, lonRight),
                            std::min(latTop, latBottom),
                            std::max(lonLeft, lonRight),
                            std::max(latTop, latBottom),
                            types,
                            nodes,
                            ways,
                            areas,
                            relationWays,
                            relationAreas))
  {
    std::cerr << "Cannot load data from database" << std::endl;

    return false;
  }

  for (std::vector<osmscout::NodeRef>::const_iterator node = nodes.begin();
       node != nodes.end();
       node++)
  {
    std::string name;

    if (nameTagId != osmscout::tagIgnore)
    {
      for (size_t i = 0; i < (*node)->GetTagCount(); i++)
      {
        if ((*node)->GetTagKey(i) == nameTagId)
        {
          name = (*node)->GetTagValue(i);
          break;
        }
      }
    }

    std::cout << "+ Node " << (*node)->GetId();
    std::cout << " " << mDatabase.GetTypeConfig()->GetTypeInfo((*node)->GetType()).GetName();
    std::cout << " " << name << std::endl;
  }

  for (std::vector<osmscout::WayRef>::const_iterator way = ways.begin();
       way != ways.end();
       way++)
  {
    std::cout << "+ Way " << (*way)->GetId();
    std::cout << " " << mDatabase.GetTypeConfig()->GetTypeInfo((*way)->GetType()).GetName();
    std::cout << " " << (*way)->GetName();
    double lat = 0;
    double lon = 0;
    (*way)->GetCenter(lat, lon);
    std::cout << " lat=" << lat << " lon=" << lon << std::endl;


    for (std::vector<osmscout::Point>::const_iterator point = (*way)->nodes.begin();
         point != (*way)->nodes.end();
         point++)
    {
      cout << "-> way node: lat=" << (*point).GetLat() << " lon=" << (*point).GetLon() << endl;

      Vector2d wayNodePosVec((*point).GetLat(), (*point).GetLon());

      double distance = vectorDistance(currentPosVec, wayNodePosVec);
      cout << "-> distance to car: " << distance << endl;

      if (distance < maxDistance)
      {
        foundWay = (*way);
        foundWayPoint = (*point);
        maxDistance = distance;
        ret = true;
      }
    }
  }

  for (std::vector<osmscout::RelationRef>::const_iterator way = relationWays.begin();
       way != relationWays.end();
       way++)
  {
    std::cout << "+ Way Reference " << (*way)->GetId();
    std::cout << " " << mDatabase.GetTypeConfig()->GetTypeInfo((*way)->GetType()).GetName();
    std::cout << " " << (*way)->GetName() << std::endl;
  }

  for (std::vector<osmscout::WayRef>::const_iterator area = areas.begin();
       area != areas.end();
       area++)
  {
    std::cout << "+ Area " << (*area)->GetId();
    std::cout << " " << mDatabase.GetTypeConfig()->GetTypeInfo((*area)->GetType()).GetName();
    std::cout << " " << (*area)->GetName() << std::endl;
  }

  for (std::vector<osmscout::RelationRef>::const_iterator area = relationAreas.begin();
       area != relationAreas.end();
       area++)
  {
    std::cout << "+ Area " << (*area)->GetId();
    std::cout << " " << mDatabase.GetTypeConfig()->GetTypeInfo((*area)->GetType()).GetName();
    std::cout << " " << (*area)->GetName() << std::endl;
  }

  // return by out parameter
  /*if (ways.size () >= 1)
  {
    foundWay = (*ways.begin());
    return true;
  }*/

  return ret;
}

void MapCanvas::printRouteList(osmscout::RouteData &route)
{
  osmscout::RouteDescription description;

  mRouter.TransformRouteDataToRouteDescription(route, description);

/*  for (std::list <osmscout::RouteDescription::RouteStep>::const_iterator step = description.Steps().begin();
       step != description.Steps().end();
       ++step)
  {

    std::cout << std::fixed << std::setprecision(1);
    std::cout << step->GetAt() << "km ";

    if (step->GetAfter() != 0.0)
    {
      std::cout << std::fixed << std::setprecision(1);
      std::cout << step->GetAfter() << "km ";
    }
    else
    {
      std::cout << "      ";
    }

    switch (step->GetAction())
    {
    case osmscout::RouteDescription::start:
      std::cout << "Start at ";
      if (!step->GetName().empty())
      {
        std::cout << step->GetName();

        if (!step->GetRefName().empty())
        {
          std::cout << " (" << step->GetRefName() << ")";
        }
      }
      else
      {
        std::cout << step->GetRefName();
      }
      break;
    case osmscout::RouteDescription::drive:
      std::cout << "drive along ";
      if (!step->GetName().empty())
      {
        std::cout << step->GetName();

        if (!step->GetRefName().empty())
        {
          std::cout << " (" << step->GetRefName() << ")";
        }
      }
      else
      {
        std::cout << step->GetRefName();
      }
      break;
    case osmscout::RouteDescription::switchRoad:
      std::cout << "turn into ";
      if (!step->GetName().empty())
      {
        std::cout << step->GetName();

        if (!step->GetRefName().empty())
        {
          std::cout << " (" << step->GetRefName() << ")";
        }
      }
      else
      {
        std::cout << step->GetRefName();
      }
      break;
    case osmscout::RouteDescription::reachTarget:
      std::cout << "Arriving at ";
      if (!step->GetName().empty())
      {
        std::cout << step->GetName();

        if (!step->GetRefName().empty())
        {
          std::cout << " (" << step->GetRefName() << ")";
        }
      }
      else
      {
        std::cout << step->GetRefName();
      }
      break;
    case osmscout::RouteDescription::pass:
      std::cout << "passing along ";
      if (!step->GetName().empty())
      {
        std::cout << step->GetName();

        if (!step->GetRefName().empty())
        {
          std::cout << " (" << step->GetRefName() << ")";
        }
      }
      else
      {
        std::cout << step->GetRefName();
      }
      break;
    }

    std::cout << std::endl;
  }*/
}

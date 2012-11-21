#include "Preferences.h"

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

using namespace std;

Preferences& Preferences::instance()
{
  static Preferences g;
  return g;
}

void Preferences::init ()
{
  mGPSDHost = "localhost"; // default gpsd host
  mGPSDPort = 2947; // default gpsd port
  mDesktopLayer = false; 
  mNaviMapFolder = string(PACKAGE_DATA_DIR) +  "/osmscout/map/current/"; // FIXME: hard coded developer path, put this to config file
}

void Preferences::setGPSHost(const std::string &host)
{
  mGPSDHost = host;
}

std::string Preferences::getGPSDHost()
{
  return mGPSDHost; 
}

void Preferences::setGPSDPort(int port)
{
  mGPSDHost = port;
}

int Preferences::getGPSDPort()
{
  return mGPSDPort;
}

void Preferences::setDesktopLayer(bool desktop)
{
  mDesktopLayer = desktop;
}

bool Preferences::getDesktopLayer()
{
  return mDesktopLayer;
}

void Preferences::setNaviMapFolder(const std::string &folder)
{
  mNaviMapFolder = folder;
}

std::string Preferences::getNaviMapFolder() 
{
  return mNaviMapFolder;
}

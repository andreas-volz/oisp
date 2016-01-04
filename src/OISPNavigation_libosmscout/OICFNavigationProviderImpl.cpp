#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>

/* local */
#include "Navigation.h"
#include "OICFNavigationProviderImpl.h"

using namespace std;

OICFNavigationProviderImpl::OICFNavigationProviderImpl(DBus::Connection &connection) :
  OICFNavigationProvider(connection),
  mLogger("oisp.Navigation.OICFNavigationProviderImpl")
{

}

void OICFNavigationProviderImpl::setNavigation(Navigation *navigation)
{
  m_navigation = navigation;
}

void OICFNavigationProviderImpl::moveMapSteps(const int32_t &direction, const int32_t &steps)
{
  LOG4CXX_TRACE(mLogger, "moveMapSteps()");

  m_navigation->moveMap((Navigation::PanDirection) direction);
}

void OICFNavigationProviderImpl::zoomIn()
{
  LOG4CXX_TRACE(mLogger, "zoomIn()");
  m_navigation->zoomIn();
}

void OICFNavigationProviderImpl::zoomOut()
{
  LOG4CXX_TRACE(mLogger, "zoomOut()");
  m_navigation->zoomOut();
}

void OICFNavigationProviderImpl::jumpToPointer()
{
  LOG4CXX_TRACE(mLogger, "jumpToPointer()");
  m_navigation->jumpToPointer();
}

void OICFNavigationProviderImpl::searchCity(const std::string& city)
{

}

void OICFNavigationProviderImpl::getNextValidCharacters()
{

}

void OICFNavigationProviderImpl::generic1(const std::string &param1, const std::string &param2, const std::string &param3)
{
  m_navigation->startRouteTo(param1, param2);
}

void OICFNavigationProviderImpl::generic2(const std::string &param1, const std::string &param2, const std::string &param3)
{

}

void OICFNavigationProviderImpl::generic3(const std::string &param1, const std::string &param2, const std::string &param3)
{

}

std::map< std::string, std::string > OICFNavigationProviderImpl::Info()
{
  std::map< std::string, std::string > info;
  char hostname[HOST_NAME_MAX];

  gethostname(hostname, sizeof(hostname));
  info["hostname"] = hostname;
  info["username"] = getlogin();

  return info;
}


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OICFControlListenerProviderImpl.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <climits>

using namespace std;

OICFControlListenerProviderImpl::OICFControlListenerProviderImpl(DBus::Connection &connection) :
  OICFControlListenerProvider(connection)
{
}

std::map< std::string, std::string > OICFControlListenerProviderImpl::Info()
{
  std::map< std::string, std::string > info;
  char hostname[HOST_NAME_MAX];

  gethostname(hostname, sizeof(hostname));
  info["hostname"] = hostname;
  info["username"] = getlogin();

  return info;
}


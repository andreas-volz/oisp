#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include "GPSConnection.h"
#include <cstring>
#include "util.h"

using namespace std;

// ******* global functions *******
static struct gps_data_t *gpsDataGlobal;
static struct gps_data_t gpsDataG;
static pthread_mutex_t gpsMutex = PTHREAD_MUTEX_INITIALIZER;
static GPSConnection *gpsConGlobal = NULL;

void GPSConnection::gpsCallback(struct gps_data_t *sentence, char *buf,  size_t len)
{
  struct gps_data_t gpsDataLocal;

  pthread_mutex_lock(&gpsMutex);
  memcpy(&gpsDataLocal, sentence, sizeof(struct gps_data_t));
  pthread_mutex_unlock(&gpsMutex);

  update();
}

// ******* member functions *******

GPSConnection::GPSConnection() :
  signal(false),
  connection(false)
{
  gpsConGlobal = this;
}

GPSConnection::~GPSConnection()
{
  this->close();
}

void GPSConnection::update()
{
  struct gps_data_t gpsDataLocal;

  if (gpsDataGlobal)
  {
    pthread_mutex_lock(&gpsMutex);
    memcpy(&gpsDataLocal, gpsDataGlobal, sizeof(struct gps_data_t));
    pthread_mutex_unlock(&gpsMutex);

    if (gpsConGlobal)
    {
      gpsConGlobal->signalData.emit(&gpsDataLocal);
    }
  }
}

bool GPSConnection::open(const std::string &host, int port)
{
  return this->open(host, toString(port));
}

bool GPSConnection::open(const string &host, const string &port)
{
  if (!connection)
  {
    bool ret = gps_open(host.c_str(), port.c_str(), &gpsDataG);

    if (!ret)
    {
      connection = true;
    }
  }

  return connection;
}

bool GPSConnection::stream(unsigned int flags)
{
  int retVal = -1;

  if (connection)
  {
    struct gps_data_t gpsDataLocal;

    pthread_mutex_lock(&gpsMutex);
    memcpy(&gpsDataLocal, &gpsDataG, sizeof(struct gps_data_t));
    pthread_mutex_unlock(&gpsMutex);

    retVal = gps_stream(&gpsDataLocal, flags, NULL);
  }

  return !retVal;
}

void GPSConnection::close()
{
  if (connection)
  {
    gps_close(gpsDataGlobal);
    connection = false;
  }
}

bool GPSConnection::read(struct gps_data_t *outGPSData)
{
  int retVal = -1;

  if(connection)
  {
    if(gps_waiting (&gpsDataG, 500))
    {
      errno = 0;
      if(gps_read (&gpsDataG) == -1)
      {

      } 
      else
      {
        *outGPSData = gpsDataG;
      }
    }
  }

  return !retVal;
}

void GPSConnection::setSignaling(bool inSignal)
{
  signal = inSignal;

  if (inSignal)
  {

  }
  else
  {

  }
}

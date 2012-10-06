#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include "GPSConnection.h"
#include <cstring>

using namespace std;

// ******* global functions *******
static struct gps_data_t *gpsDataGlobal;
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

bool GPSConnection::open()
{
  return this->open("localhost", "122333"); // 2947
}

bool GPSConnection::open(const string &host, const string &port)
{
  if (!connection)
  {
    bool ret = gps_open(host.c_str(), port.c_str(), gpsDataGlobal);

    if (!ret)
    {
      connection = true;
    }
  }

  return connection;
}

bool GPSConnection::query(unsigned int flags)
{
  int retVal = -1;

  if (connection)
  {
    struct gps_data_t gpsDataLocal;

    pthread_mutex_lock(&gpsMutex);
    memcpy(&gpsDataLocal, gpsDataGlobal, sizeof(struct gps_data_t));
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

bool GPSConnection::poll(struct gps_data_t *outGPSData)
{
  int retVal = -1;

  if(connection)
  {
    //retVal = gps_poll(gpsDataGlobal);
    if(gps_waiting (gpsDataGlobal, 500))
    {
      errno = 0;
      if(gps_read (gpsDataGlobal) == -1)
      {

      } 
      else
      {
        outGPSData = gpsDataGlobal;
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
    //gps_set_raw_hook(gpsDataGlobal, &gpsCallback);
  }
  else
  {
    //gps_set_raw_hook(gpsDataGlobal, NULL);
  }
}

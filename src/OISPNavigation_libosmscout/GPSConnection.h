#ifndef GPSCONNECTION_H
#define GPSCONNECTION_H

#include <sigc++/sigc++.h>
#include <gps.h>

// libgps porting Guide:
// http://gpsd.berlios.de/protocol-transition.html

// TODO: - think about wrapping libgpsmm
//       - signaling doesn't work at the moment!

using std::string;

/*!
 * Attention: You need to dispatch the signal if you're
 * e.g. doing async GUI updates.
 */
class GPSConnection : public sigc::trackable
{
public:
  sigc::signal <void, struct gps_data_t *> signalData;

  GPSConnection();
  virtual ~GPSConnection();

  /*!
   * Open a connection to a gpsd on localhost:2947.
   */
  bool open();

  /*! Open a connection to a gpsd on given host and port.
   * @param host The host where gpsd runs.
   * @param port The port where gpsd runs.
   */
  bool open(const string &host, const string &port);

  /*! Closes the gpsd connection.
   */
  void close();

  /*! Query a request to gpsd.
   * @param request The request string.
   */
  bool query(unsigned int flags);

  /*! Poll only a single GPS position.
   * @param outGPSData Get the GPS data from gpsd.
   */
  bool poll(struct gps_data_t *outGPSData);

  /*! Set if the gpsd callback should be active
   * @param inSignal Set true for active and false for inactive
   */
  void setSignaling(bool inSignal);

private:
  pthread_t gpsThread;
  bool signal;
  bool connection;

  static void gpsCallback(struct gps_data_t *sentence, char *buf, size_t len);
  static void update();
};

#endif // GPSCONNECTION_H

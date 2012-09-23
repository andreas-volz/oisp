#ifndef ECOREDISPATCHER_H
#define ECOREDISPATCHER_H

#include <Ecore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sigc++/sigc++.h>

class EcoreDispatcher: public sigc::trackable
{
public:
  /*!
   * @param int number of missed events since last async event
   */
  sigc::signal <void, int> signalDispatch;

  EcoreDispatcher();
  virtual ~EcoreDispatcher();

  void signal();

private:
  int fd_read;
  int fd_write;

  void init();
  static Eina_Bool dispatcher_async_handler(void *data, Ecore_Fd_Handler *fdh);
};

#endif // ECOREDISPATCHER_H

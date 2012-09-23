#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "EcoreDispatcher.h"
#include <iostream>

using namespace std;

EcoreDispatcher::EcoreDispatcher()
{
  init();
}

EcoreDispatcher::~EcoreDispatcher()
{

}

void EcoreDispatcher::init()
{
  int fd[2];
  Ecore_Fd_Handler *fd_handler;

  // Create the file descriptors
  if (pipe(fd) == 0)
  {
    fd_read = fd[0];
    fd_write = fd[1];
    fcntl(fd_read, F_SETFL, O_NONBLOCK);
    fd_handler = ecore_main_fd_handler_add(fd_read,
                                           ECORE_FD_READ,
                                           dispatcher_async_handler,
                                           this,
                                           NULL, NULL);
    ecore_main_fd_handler_active_set(fd_handler, ECORE_FD_READ);
  }
  else
  {
    cerr << "pipe() failed" << endl;
    exit(1);
  }
}

void EcoreDispatcher::signal()
{
  ssize_t len = write(fd_write, "1", 2);
}

Eina_Bool EcoreDispatcher::dispatcher_async_handler(void *data, Ecore_Fd_Handler *fdh)
{
  int fd;
  char buf[1];
  EcoreDispatcher *dispatcher = static_cast <EcoreDispatcher *>(data);

  fd = ecore_main_fd_handler_fd_get(fdh);

  int recBytes;
  int missedEvents = 0;
  do
  {
    recBytes = read(fd, buf, sizeof(buf));
    missedEvents++;
  }
  while (recBytes > 0);

  dispatcher->signalDispatch.emit(missedEvents);

  return 1;
}

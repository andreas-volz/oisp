#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Simulation.h"

int main(int argc, const char **argv)
{
  Simulation simu;

  return simu.exec(argc, argv);
}


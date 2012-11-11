#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

/* libosmscout */
#include <osmscout/Database.h>
#include <osmscout/MapPainter.h>
#include <osmscout/StyleConfigLoader.h>

/* STD */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <iostream>

/* DBus-C++ */
#include <dbus-c++/dbus.h>
#include <dbus-c++/ecore-integration.h>

/* local */
#include "ScreenManager.h"
#include "NavigationScreen.h"
#include "Navigation.h"
#include "OICFNavigationProviderImpl.h"
#include "optionparser.h"
#include "Preferences.h"

static const char *MAP_VIEWER_NAME = "org.oicf.Navigation";

using namespace std;
using namespace Eflxx;

static const Eflxx::Size initialWindowSize(800, 480);

DBus::Ecore::BusDispatcher dispatcher;

void quit()
{
  cout << "quit()" << endl;
  
  Ecorexx::Application::quit();
}

void sig_exit(int sig)
{
  quit();
}

/** option parser **/

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum  optionIndex { UNKNOWN, HELP, DESKTOP, GPSDPORT, NAVIMAP };
 const option::Descriptor usage[] =
 {
  {UNKNOWN, 0,"" , ""    ,option::Arg::None, "USAGE: example [options]\n\n"
                                             "Options:" },
  {HELP,    0,"h" , "help",option::Arg::None, "  --help, -h  \tPrint usage and exit." },
  {DESKTOP,0,"d" , "desktop",option::Arg::None, "  --desktop, -d  \tPin application to desktop layer." },
  {GPSDPORT, 0,"g" , "gpsd-port",Arg::Numeric, "  --gpsd-port, -g  \tUser specific gpsd port (default: 2947)." },
  {NAVIMAP, 0,"n" , "navimap",Arg::Required, "  --navimap, -n  \tSet folder to search osmscout map." },
  {UNKNOWN, 0,""  ,  ""   ,option::Arg::None, "\nExamples:\n"
                                             "  example --unknown -- --this_is_no_option\n"
                                             "  example -unk --plus -ppp file1 file2\n" },
  {0,0,0,0,0,0}
 };

int parseOptions(int argc, char **argv)
{
  Preferences &preferences = Preferences::instance ();
  argc -= (argc > 0); argv += (argc > 0); // skip program name argv[0] if present
  option::Stats  stats(usage, argc, argv);
  option::Option options[stats.options_max], buffer[stats.buffer_max];
  option::Parser parse(usage, argc, argv, options, buffer);

  if(parse.error())
    quit();

  if(options[HELP])
  {
    option::printUsage(std::cout, usage);
    quit();
  }

  // parse options
  if(options[DESKTOP].count() > 0)
  {
    preferences.setDesktopLayer(true);
  }

  if(options[GPSDPORT].count() > 0)
  {
    preferences.setGPSDPort(atoi(options[GPSDPORT].arg));
  }

  if(options[NAVIMAP].count() > 0)
  {
    preferences.setNaviMapFolder(options[NAVIMAP].arg);
  }
 
  for(option::Option* opt = options[UNKNOWN]; opt; opt = opt->next())
    std::cout << "Unknown option: " << opt->name << "\n";

  for(int i = 0; i < parse.nonOptionsCount(); ++i)
    std::cout << "Non-option #" << i << ": " << parse.nonOption(i) << "\n";

  return 0;
}

/** option parser **/ 

Eina_Bool eina_list_init(void);

int main(int argc, char **argv)
{
  signal(SIGTERM, sig_exit);
  signal(SIGINT, sig_exit);

  Preferences &preferences = Preferences::instance ();
  preferences.init ();
  
  parseOptions(argc, argv);

  // create and init ScreenManager (and Ecore!!)
  ScreenManager &screenManager(ScreenManager::instance());
  screenManager.init(argc, argv, initialWindowSize);

  // initialize Glib thread system
  if (!Glib::thread_supported()) Glib::thread_init();

  //DBus::_init_threading(); ??

  // DBus-C++ stuff first
  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();
  conn.request_name(MAP_VIEWER_NAME);

  // send signals to GUI with this...
  OICFNavigationListenerProvider mapViewerListenerProvider(conn);

  // get signals from GUI through this...
  OICFNavigationProviderImpl mapProvider(conn);

  // create screen after ScreenManager initialization!
  NavigationScreen navigationScreen;

  // create navigation object after DBus and screen init
  Navigation navigation (&mapViewerListenerProvider);
  navigation.initGPS(preferences.getGPSDHost(), preferences.getGPSDPort());

  navigationScreen.setNavigation(&navigation);
  mapProvider.setNavigation(&navigation);

  navigation.setNavigationScreen(&navigationScreen);

  navigation.setOICFNavigationListenerProvider(&mapViewerListenerProvider);

  navigation.start();

  cout << "OISPNavigation server started..." << endl;

  screenManager.app->exec();
  
  cout << "App Exit!" << endl;
  return 0;
}

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <math.h>
#include <iostream>
#include <signal.h>

#include "OICFMediaProviderImpl.h"
#include "Preferences.h"
#include "Player.h"
#include "searchFile.h"

/* common */
#include "../common/optionparser.h"
#include "../common/Logger.h"

/* EFLxx */
#include <ecorexx/Ecorexx.h>
#include <emotionxx/Emotionxx.h>

#include <dbus-c++/dbus.h>
#include <glibmm.h>
#include <dbus-c++/ecore-integration.h>

static const char *SERVER_NAME = "org.oicf.Media";

using namespace std;
using namespace Eflxx;

DBus::Ecore::BusDispatcher dispatcher;

void quit()
{
  cout << "quit()" << endl;
  
  Ecorexx::Application::quit();
  exit(0);
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

enum  optionIndex { UNKNOWN, HELP, ENGINE, MUSIC };
 const option::Descriptor usage[] =
 {
  {UNKNOWN, 0,"" , ""    ,option::Arg::None, "USAGE: example [options]\n\n"
                                             "Options:" },
  {HELP,    0,"h" , "help",option::Arg::None, "  --help, -h  \tPrint usage and exit." },
  {ENGINE,0,"e" , "engine",Arg::Required, "  --engine, -e  \tLoad a emotion engine." },
  {MUSIC, 0,"m" , "music-root",Arg::Required, "  --music-root, -m  \tSet folder to search music data." },
  {UNKNOWN, 0,""  ,  ""   ,option::Arg::None, "\nExamples:\n"
                                             "  example --unknown -- --this_is_no_option\n"
                                             "  example -unk --plus -ppp file1 file2\n" },
  {0,0,0,0,0,0}
 };

int parseOptions(int argc, const char **argv)
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
  if(options[ENGINE].count() > 0)
  {
    preferences.setEmotionEngine(options[ENGINE].arg);
  }
  
  if(options[MUSIC].count() > 0)
  {
    preferences.setMusicRootFolder(options[MUSIC].arg);
  }
 
  for(option::Option* opt = options[UNKNOWN]; opt; opt = opt->next())
    std::cout << "Unknown option: " << opt->name << "\n";

  for(int i = 0; i < parse.nonOptionsCount(); ++i)
    std::cout << "Non-option #" << i << ": " << parse.nonOption(i) << "\n";

  return 0;
}

/** option parser **/ 


int main(int argc, const char **argv)
{
  signal(SIGTERM, sig_exit);
  signal(SIGINT, sig_exit);

#ifdef HAVE_LOG4CXX
  //cout << "searchDataDir ():" << searchDataDir() << endl;
  log4cxx::PropertyConfigurator::configure(searchDataDir() + "/logging.prop");
#endif // HAVE_LOG4CXX

  Logger logger("oisp.Media.main");

  Preferences &preferences = Preferences::instance ();
  preferences.init();
  
  parseOptions(argc, argv);

  /* Create the application object */
  Ecorexx::Application app(argc, argv, "OISPMedia");

  /* Create the main window, a window with an embedded canvas */
  Ecorexx::EvasWindowSoftwareX11 *mw = new Ecorexx::EvasWindowSoftwareX11(Size(0, 0));
  Evasxx::Canvas &evas = mw->getCanvas();

  /* Create Emotionxx::Object object using xine engine */
  Emotionxx::AudioObject *emotion = new Emotionxx::AudioObject(evas, preferences.getEmotionEngine());

  Player player(emotion);

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();
  conn.request_name(SERVER_NAME);

  OICFMediaListenerProvider mediaListenerProvider(conn);

  OICFMediaProviderImpl mediaProvider(conn);

  mediaProvider.setOICFMediaListenerProvider(&mediaListenerProvider);
  mediaProvider.setPlayer(&player);

  // needed to intialize the first startup list
  mediaProvider.getWindowList(0, 100);

  LOG4CXX_INFO(logger, "OISPMedia server started...");

  /* Enter the application main loop */
  app.exec();

  return 0;
}

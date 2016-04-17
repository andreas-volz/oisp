#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* STD */
#include <iostream>

#include "Preferences.h"

/* EFL */
#include <Efreet.h> 

using namespace std;

Preferences& Preferences::instance()
{
  static Preferences g;
  return g;
}

void Preferences::init ()
{
  mMusicRootFolder = "/home/andreas/Musik"; // efreet_music_dir_get(); => why crash?
  mEmotionEngine = "gstreamer1";
}

void Preferences::setMusicRootFolder(const std::string &folder)
{
  mMusicRootFolder = folder;
}

std::string Preferences::getMusicRootFolder() 
{
  return mMusicRootFolder;
}

void Preferences::setEmotionEngine(const std::string &engine)
{
  mEmotionEngine = engine;
}

std::string Preferences::getEmotionEngine()
{
  return mEmotionEngine;
}
